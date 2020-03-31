// File operation utilities for UTF-8 encoding support

#ifndef __BASE_EXTENSION_UTF8_FILE_UTIL_H__
#define __BASE_EXTENSION_UTF8_FILE_UTIL_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/strings/unicode.h"
#include "google_base/base/files/file_path.h"
#include "google_base/base/scoped_generic.h"
#include "google_base/base/files/file_enumerator.h"
#include <list>

EXTENSION_BEGIN_DECLS

using FilePath = base::FilePath;
using PathString = FilePath::StringType;

// Check the character is filepath separator
EXTENSION_EXPORT bool IsFilePathSeparator(const UTF8CharType separator);
EXTENSION_EXPORT bool IsFilePathSeparator(const UTF8String &separator);
#if defined(OS_WIN)
EXTENSION_EXPORT bool IsFilePathSeparator(const UTF16CharType separator);
EXTENSION_EXPORT bool IsFilePathSeparator(const UTF16String &separator);
#endif

EXTENSION_EXPORT UTF8String FilePathAsEndWithSeparator(const UTF8String &filepath_in);
#if defined(OS_WIN)
EXTENSION_EXPORT UTF16String FilePathAsEndWithSeparator(const UTF16String &filepath_in);
#endif

// Get the file extension from filepath
EXTENSION_EXPORT bool FilePathExtension(const UTF8String &filepath_in,
					   UTF8String &extension_out);
#if defined(OS_WIN)
EXTENSION_EXPORT bool FilePathExtension(const UTF16String &filepath_in,
	UTF16String &extension_out);
#endif

// Get the directory from the whole filepath
EXTENSION_EXPORT bool FilePathApartDirectory(const UTF8String &filepath_in,
							UTF8String &directory_out);
#if defined(OS_WIN)
EXTENSION_EXPORT bool FilePathApartDirectory(const UTF16String &filepath_in,
	UTF16String &directory_out);
#endif
// Get the filename from the whole filepath
EXTENSION_EXPORT bool FilePathApartFileName(const UTF8String &filepath_in,
						   UTF8String &filename_out);
#if defined(OS_WIN)
EXTENSION_EXPORT bool FilePathApartFileName(const UTF16String &filepath_in,
	UTF16String &filename_out);
#endif

// Parse and extract all components of a path
EXTENSION_EXPORT bool ParsePathComponents(const UTF8CharType *PathChar,
						 std::list<UTF8String> &components);
#if defined(OS_WIN)
EXTENSION_EXPORT bool ParsePathComponents(const UTF16CharType *PathChar,
	std::list<UTF16String> &components);
#endif

EXTENSION_EXPORT void CalculateFileMd5(const UTF8String &file_path, UTF8String &md5_value);
#if defined(OS_WIN)
EXTENSION_EXPORT void CalculateFileMd5(const UTF16String &file_path, UTF16String &md5_value);
#endif

// Check if |component| is a directory
EXTENSION_EXPORT bool IsDirectoryComponent(const UTF8String &component);
#if defined(OS_WIN)
EXTENSION_EXPORT bool IsDirectoryComponent(const UTF16String &component);
#endif
// Compose the filepath from directory and filename
EXTENSION_EXPORT bool FilePathCompose(const UTF8String &directory_in,
					 const UTF8String &filename_in,
					 UTF8String &filepath_out);
#if defined(OS_WIN)
EXTENSION_EXPORT bool FilePathCompose(const UTF16String &directory_in,
	const UTF16String &filename_in,
	UTF16String &filepath_out);
#endif

// Check the filepath is exist
// If |is_directory| is true, check a directory, or check the path
EXTENSION_EXPORT bool FilePathIsExist(const UTF8String &filepath_in, bool is_directory);
#if defined(OS_WIN)
EXTENSION_EXPORT bool FilePathIsExist(const UTF16String &filepath_in, bool is_directory);
#endif
// Create a directory, all subdirs will be created if not existing
EXTENSION_EXPORT bool CreateDirectory(const UTF8String& full_path);
#if defined(OS_WIN)
EXTENSION_EXPORT bool CreateDirectory(const UTF16String& full_path);
#endif

// Wrapper for fopen-like calls. Returns non-NULL FILE* on success
EXTENSION_EXPORT FILE* OpenFile(const UTF8String &filepath, const char *mode);
#if defined(OS_WIN)
EXTENSION_EXPORT FILE* OpenFile(const UTF16String &filepath, const char *mode);
#endif

// Reads the given number of bytes from the file into the buffer
EXTENSION_EXPORT int ReadFile(const UTF8String &filepath, void *data_out, int size);
#if defined(OS_WIN)
EXTENSION_EXPORT int ReadFile(const UTF16String &filepath, void *data_out, int size);
#endif

// Read the file at |path| into |contents|, returning true on success.
EXTENSION_EXPORT bool ReadFileToString(const UTF8String &filepath, std::string &contents_out);
#if defined(OS_WIN)
EXTENSION_EXPORT bool ReadFileToString(const UTF16String &filepath, std::string &contents_out);
#endif

// Writes the content of given buffer into the file
EXTENSION_EXPORT int WriteFile(const UTF8String &filepath, const std::string &data);
#if defined(OS_WIN)
EXTENSION_EXPORT int WriteFile(const UTF16String &filepath, const std::string &data);
#endif

EXTENSION_EXPORT bool CloseFile(FILE* file);

// Copies a single file.
EXTENSION_EXPORT bool CopyFile(const UTF8String &from_path, const UTF8String &to_path);
#if defined(OS_WIN)
EXTENSION_EXPORT bool CopyFile(const UTF16String &from_path, const UTF16String &to_path);
#endif

// Moves a single file.
EXTENSION_EXPORT bool MoveFile(const UTF8String &from_path, const UTF8String &to_path);
#if defined(OS_WIN)
EXTENSION_EXPORT bool MoveFile(const UTF16String &from_path, const UTF16String &to_path);
#endif

// Deletes the given path.
EXTENSION_EXPORT bool DeleteFile(const UTF8String &filepath);
#if defined(OS_WIN)
EXTENSION_EXPORT bool DeleteFile(const UTF16String &filepath);
#endif

// Get file size.
EXTENSION_EXPORT int64_t GetFileSize(const UTF8String &filepath);
#if defined(OS_WIN)
EXTENSION_EXPORT int64_t GetFileSize(const UTF16String &filepath);
#endif

using ExtensionFile = decltype(OpenFile("",""));
struct EXTENSION_EXPORT ScopedFileTraits {
	static ExtensionFile InvalidValue();
	static void Free(ExtensionFile fd);
};
class EXTENSION_EXPORT ScopedFile : public base::ScopedGeneric<ExtensionFile, ScopedFileTraits>
{
	using MyParent = base::ScopedGeneric<ExtensionFile, ScopedFileTraits>;
public:
	ScopedFile() {}
	ScopedFile(const UTF8String& path, const UTF8String& open_flag);
#if defined(OS_WIN)
	ScopedFile(const UTF16String& path, const UTF16String& open_flag);
#endif
	operator ExtensionFile() const
	{
		return MyParent::get();
	}
};

class FileEnumerator : public base::FileEnumerator
{
public:
	FileEnumerator(const UTF8String& root_path, bool recursive,int file_type, const UTF8String& pattern);
};
EXTENSION_END_DECLS

#endif // __BASE_EXTENSION_UTF8_FILE_UTIL_H__
