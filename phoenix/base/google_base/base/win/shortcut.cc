// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/shortcut.h"

#include <shellapi.h>
#include <shlobj.h>
#include <propkey.h>

#include "base/files/file_util.h"
#include "base/threading/thread_restrictions.h"
#include "base/win/scoped_comptr.h"
#include "base/win/scoped_propvariant.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"

namespace base {
namespace win {

namespace {

// Initializes |i_shell_link| and |i_persist_file| (releasing them first if they
// are already initialized).
// If |shortcut| is not NULL, loads |shortcut| into |i_persist_file|.
// If any of the above steps fail, both |i_shell_link| and |i_persist_file| will
// be released.
void InitializeShortcutInterfaces(
    const wchar_t* shortcut,
    ScopedComPtr<IShellLink>* i_shell_link,
    ScopedComPtr<IPersistFile>* i_persist_file) {
  i_shell_link->Release();
  i_persist_file->Release();
  if (FAILED(i_shell_link->CreateInstance(CLSID_ShellLink, NULL,
                                          CLSCTX_INPROC_SERVER)) ||
      FAILED(i_persist_file->QueryFrom(i_shell_link->get())) ||
      (shortcut && FAILED((*i_persist_file)->Load(shortcut, STGM_READWRITE)))) {
    i_shell_link->Release();
    i_persist_file->Release();
  }
}

}  // namespace

ShortcutProperties::ShortcutProperties()
    : icon_index(-1), dual_mode(false), options(0U) {
}

ShortcutProperties::~ShortcutProperties() {
}

bool ResolveShortcutProperties(const FilePath& shortcut_path,
                               uint32_t options,
                               ShortcutProperties* properties) {
  DCHECK(options && properties);
  base::ThreadRestrictions::AssertIOAllowed();

  if (options & ~ShortcutProperties::PROPERTIES_ALL)
    NOTREACHED() << "Unhandled property is used.";

  ScopedComPtr<IShellLink> i_shell_link;

  // Get pointer to the IShellLink interface.
  if (FAILED(i_shell_link.CreateInstance(CLSID_ShellLink, NULL,
                                         CLSCTX_INPROC_SERVER))) {
    return false;
  }

  ScopedComPtr<IPersistFile> persist;
  // Query IShellLink for the IPersistFile interface.
  if (FAILED(persist.QueryFrom(i_shell_link.get())))
    return false;

  // Load the shell link.
  if (FAILED(persist->Load(shortcut_path.value().c_str(), STGM_READ)))
    return false;

  // Reset |properties|.
  properties->options = 0;

  wchar_t temp[MAX_PATH];
  if (options & ShortcutProperties::PROPERTIES_TARGET) {
    if (FAILED(i_shell_link->GetPath(temp, MAX_PATH, NULL, SLGP_UNCPRIORITY)))
      return false;
    properties->set_target(FilePath(temp));
  }

  if (options & ShortcutProperties::PROPERTIES_WORKING_DIR) {
    if (FAILED(i_shell_link->GetWorkingDirectory(temp, MAX_PATH)))
      return false;
    properties->set_working_dir(FilePath(temp));
  }

  if (options & ShortcutProperties::PROPERTIES_ARGUMENTS) {
    if (FAILED(i_shell_link->GetArguments(temp, MAX_PATH)))
      return false;
    properties->set_arguments(temp);
  }

  if (options & ShortcutProperties::PROPERTIES_DESCRIPTION) {
    // Note: description length constrained by MAX_PATH.
    if (FAILED(i_shell_link->GetDescription(temp, MAX_PATH)))
      return false;
    properties->set_description(temp);
  }

  if (options & ShortcutProperties::PROPERTIES_ICON) {
    int temp_index;
    if (FAILED(i_shell_link->GetIconLocation(temp, MAX_PATH, &temp_index)))
      return false;
    properties->set_icon(FilePath(temp), temp_index);
  }

  // Windows 7+ options, avoiding unnecessary work.
  if ((options & ShortcutProperties::PROPERTIES_WIN7) &&
      GetVersion() >= VERSION_WIN7) {
    ScopedComPtr<IPropertyStore> property_store;
    if (FAILED(property_store.QueryFrom(i_shell_link.get())))
      return false;

    if (options & ShortcutProperties::PROPERTIES_APP_ID) {
      ScopedPropVariant pv_app_id;
      if (property_store->GetValue(PKEY_AppUserModel_ID,
                                   pv_app_id.Receive()) != S_OK) {
        return false;
      }
      switch (pv_app_id.get().vt) {
        case VT_EMPTY:
          properties->set_app_id(L"");
          break;
        case VT_LPWSTR:
          properties->set_app_id(pv_app_id.get().pwszVal);
          break;
        default:
          NOTREACHED() << "Unexpected variant type: " << pv_app_id.get().vt;
          return false;
      }
    }

    if (options & ShortcutProperties::PROPERTIES_DUAL_MODE) {
      ScopedPropVariant pv_dual_mode;
      if (property_store->GetValue(PKEY_AppUserModel_IsDualMode,
                                   pv_dual_mode.Receive()) != S_OK) {
        return false;
      }
      switch (pv_dual_mode.get().vt) {
        case VT_EMPTY:
          properties->set_dual_mode(false);
          break;
        case VT_BOOL:
          properties->set_dual_mode(pv_dual_mode.get().boolVal == VARIANT_TRUE);
          break;
        default:
          NOTREACHED() << "Unexpected variant type: " << pv_dual_mode.get().vt;
          return false;
      }
    }
  }

  return true;
}

bool ResolveShortcut(const FilePath& shortcut_path,
                     FilePath* target_path,
                     string16* args) {
  uint32_t options = 0;
  if (target_path)
    options |= ShortcutProperties::PROPERTIES_TARGET;
  if (args)
    options |= ShortcutProperties::PROPERTIES_ARGUMENTS;
  DCHECK(options);

  ShortcutProperties properties;
  if (!ResolveShortcutProperties(shortcut_path, options, &properties))
    return false;

  if (target_path)
    *target_path = properties.target;
  if (args)
    *args = properties.arguments;
  return true;
}

bool CanPinShortcutToTaskbar() {
  // "Pin to taskbar" appeared in Windows 7 and stopped being supported in
  // Windows 10.
  return GetVersion() >= VERSION_WIN7 && GetVersion() < VERSION_WIN10;
}

bool PinShortcutToTaskbar(const FilePath& shortcut) {
  base::ThreadRestrictions::AssertIOAllowed();
  DCHECK(CanPinShortcutToTaskbar());

  intptr_t result = reinterpret_cast<intptr_t>(ShellExecute(
      NULL, L"taskbarpin", shortcut.value().c_str(), NULL, NULL, 0));
  return result > 32;
}

bool UnpinShortcutFromTaskbar(const FilePath& shortcut) {
  base::ThreadRestrictions::AssertIOAllowed();

  // "Unpin from taskbar" is only supported after Win7. It is possible to remove
  // a shortcut pinned by a user on Windows 10+.
  if (GetVersion() < VERSION_WIN7)
    return false;

  intptr_t result = reinterpret_cast<intptr_t>(ShellExecute(
      NULL, L"taskbarunpin", shortcut.value().c_str(), NULL, NULL, 0));
  return result > 32;
}

}  // namespace win
}  // namespace base
