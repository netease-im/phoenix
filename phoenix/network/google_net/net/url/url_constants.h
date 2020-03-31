// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef URL_URL_CONSTANTS_H_
#define URL_URL_CONSTANTS_H_

#include "net/net_export.h"

namespace url {

NET_EXPORT extern const char kAboutBlankURL[];

NET_EXPORT extern const char kAboutScheme[];
NET_EXPORT extern const char kBlobScheme[];
// The content scheme is specific to Android for identifying a stored file.
NET_EXPORT extern const char kContentScheme[];
NET_EXPORT extern const char kDataScheme[];
NET_EXPORT extern const char kFileScheme[];
NET_EXPORT extern const char kFileSystemScheme[];
NET_EXPORT extern const char kFtpScheme[];
NET_EXPORT extern const char kGopherScheme[];
NET_EXPORT extern const char kHttpScheme[];
NET_EXPORT extern const char kHttpsScheme[];
NET_EXPORT extern const char kJavaScriptScheme[];
NET_EXPORT extern const char kMailToScheme[];
NET_EXPORT extern const char kWsScheme[];
NET_EXPORT extern const char kWssScheme[];

// Used to separate a standard scheme and the hostname: "://".
NET_EXPORT extern const char kStandardSchemeSeparator[];

}  // namespace url

#endif  // URL_URL_CONSTANTS_H_
