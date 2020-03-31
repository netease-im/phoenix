// File operation utilities for UTF-8 encoding support
#include "extension/config/build_config.h"
#include "extension/extension_export.h"
#include "extension/file_util/utf8_file_util.h"
#include "extension/strings/string_util.h"
#include "base/files/file_util.h"
#include "base/md5.h"

EXTENSION_BEGIN_DECLS

static const UTF8CharType kEndChar = '\0';
#if defined(OS_WIN)
static const UTF8CharType kFilePathSeparators[] = "\\/";
#else
static const UTF8CharType kFilePathSeparators[] = "/";
#endif
static const UTF8CharType kFilePathCurrentDirectory[]  = ".";
static const UTF8CharType kFilePathParentDirectory[]   = "..";
static const UTF8CharType kFilePathExtensionSeparator  = '.';

#if defined(OS_WIN)
static const UTF16CharType kEndChar_UTF16 = L'\0';
static const UTF16CharType kFilePathSeparators_UTF16[] = L"\\/";
static const UTF16CharType kFilePathCurrentDirectory_UTF16[] = L".";
static const UTF16CharType kFilePathParentDirectory_UTF16[] = L"..";
static const UTF16CharType kFilePathExtensionSeparator_UTF16 = L'.';
#endif

bool IsFilePathSeparator(const UTF8CharType separator)
{
	if (separator == kEndChar)
		return false;

	size_t len = sizeof(kFilePathSeparators)/sizeof(UTF8CharType);
	for (size_t i = 0; i < len; i++)
	{
		if (separator == kFilePathSeparators[i])
			return true;
	}

	return false;
}

bool IsFilePathSeparator(const UTF8String &separator)
{
	if (separator.empty())
		return false;
	UTF8CharType c = separator[0];
	return IsFilePathSeparator(c);
}
#if defined(OS_WIN)
bool IsFilePathSeparator(const UTF16CharType separator)
{
	if (separator == kEndChar_UTF16)
		return false;
	size_t len = sizeof(kFilePathSeparators_UTF16) / sizeof(UTF16CharType);
	for (size_t i = 0; i < len; i++)
	{
		if (separator == kFilePathSeparators_UTF16[i])
			return true;
	}
	return false;
}
bool IsFilePathSeparator(const UTF16String &separator)
{
	return IsFilePathSeparator(NS_EXTENSION::UTF16ToUTF8(separator));
}
#endif
UTF8String FilePathAsEndWithSeparator(const UTF8String &filepath_in)
{
	return base::FilePath::FromUTF8Unsafe(filepath_in).AsEndingWithSeparator().AsUTF8Unsafe();
}
#if defined(OS_WIN)
UTF16String FilePathAsEndWithSeparator(const UTF16String &filepath_in)
{
    UTF8String utf8 = FilePathAsEndWithSeparator(NS_EXTENSION::UTF16ToUTF8(filepath_in));
    return NS_EXTENSION::UTF8ToUTF16(utf8);
}
#endif
bool FilePathExtension(const UTF8String &filepath_in,
					   UTF8String &extension_out)
{
	if (filepath_in.size() == 0)
		return false;
	bool ret = false;
	UTF8String file_name;
	if (FilePathApartFileName(filepath_in, file_name))
	{
		size_t pos = file_name.rfind(kFilePathExtensionSeparator);
		if (pos != UTF8String::npos)
		{
			extension_out = file_name.substr(pos, UTF8String::npos);
			ret = true;
		}			
	}	
	return ret;
}
#if defined(OS_WIN)
bool FilePathExtension(const UTF16String &filepath_in,	UTF16String &extension_out)
{
	UTF8String out;
	auto ret = FilePathExtension(NS_EXTENSION::UTF16ToUTF8(filepath_in), out);
	if (ret)
		extension_out = NS_EXTENSION::UTF8ToUTF16(out);
	return ret;
}
#endif
bool FilePathApartDirectory(const UTF8String &filepath_in,
							UTF8String &directory_out)
{
	size_t index = filepath_in.size() - 1;
	if (index <= 0 || filepath_in.size() == 0)
		return false;
	for (; index != 0; index--)
	{
		if (IsFilePathSeparator(filepath_in[index]))
		{
			if (index == filepath_in.size() - 1)
				directory_out = filepath_in;
			else
				directory_out = filepath_in.substr(0, index + 1);
			return true;
		}
	}
	return false;
}
#if defined(OS_WIN)
bool FilePathApartDirectory(const UTF16String &filepath_in, UTF16String &directory_out)
{
    UTF8String out;
	if (FilePathApartDirectory(NS_EXTENSION::UTF16ToUTF8(filepath_in), out))
	{
		directory_out = NS_EXTENSION::UTF8ToUTF16(out);
		return true;
	}
	return false;
}
#endif
// Get the filename from the whole filepath
bool FilePathApartFileName(const UTF8String &filepath_in,
						   UTF8String &filename_out)
{
	if (filepath_in.size() == 0)
		return false;
	bool ret = true;
	size_t separator_pos = UTF8String::npos;
	size_t separators_count = sizeof(kFilePathSeparators) / sizeof(UTF8CharType);
	for (size_t index = 0; index < separators_count; index++)
	{
		separator_pos = filepath_in.rfind(kFilePathSeparators[index]);
		if (separator_pos != UTF8String::npos)
			break;
	}
	if (separator_pos++ != UTF8String::npos && separator_pos < filepath_in.size())
		filename_out = filepath_in.substr(separator_pos);
	else if (separator_pos >= filepath_in.size())
		ret = false;
	else
		filename_out = filepath_in;
	return ret;
}
#if defined(OS_WIN)
bool FilePathApartFileName(const UTF16String &filepath_in, UTF16String &filename_out)
{
	if (UTF8String out;FilePathApartFileName(NS_EXTENSION::UTF16ToUTF8(filepath_in), out))
	{
		filename_out = NS_EXTENSION::UTF8ToUTF16(out);
		return true;
	}
	return false;
}
#endif
template<typename CharType>
bool ParsePathComponentsT(const CharType *path,
						  const CharType *seperators,
						  std::list<std::basic_string<CharType> > &components)
{
	components.clear();
	if (path == NULL)
		return false;
	const CharType *prev = NULL;
	const CharType *next = path;
	const CharType *c;
	while (*next)
	{
		prev = next;
		// find the first seperator
		for (;;)
		{
			for (c = seperators; *c && *next != *c; c++);
			if (*c || !*next)
				break;
			next++;
		}
		components.push_back(std::basic_string<CharType>(prev, next - prev));
		if (*next)
			components.back().push_back(*seperators);
		// skip duplicated seperators
		for (++next;;)
		{
			for (c = seperators; *c && *next != *c; c++);
			if (!*c)
				break;
			next++;
		}
	}
	return true;
}

bool ParsePathComponents(const UTF8CharType *path,
						 std::list<UTF8String> &components)
{
	return ParsePathComponentsT<UTF8CharType>(path,
										  kFilePathSeparators,
										  components);
}
#if defined(OS_WIN)
bool ParsePathComponents(const UTF16CharType *PathChar,std::list<UTF16String> &components)
{
	return ParsePathComponentsT<UTF16CharType>(PathChar,
		kFilePathSeparators_UTF16,
		components);
}
#endif
void CalculateFileMd5(const UTF8String &file_path, UTF8String &md5_value)
{
	char buffer[1024];
	FILE* fp = NS_EXTENSION::OpenFile(file_path, "rb");
	if (fp == NULL)
		return;
	base::MD5Context mm;
	base::MD5Init(&mm);
	int len = 0;
	while (!feof(fp))
	{
		memset(buffer, 0, 1024 * sizeof(char));
		if ((int)fread(buffer, sizeof(char), 1024, fp) > 0)
		{
			base::MD5Update(&mm, buffer);			
			continue;
		}
		break;
	}
	NS_EXTENSION::CloseFile(fp);
	base::MD5Digest md5;
	base::MD5Final(&md5, &mm);
	constexpr int md5_length = 16;
	BinaryToHexString(md5.a, md5_length, md5_value);
}
#if defined(OS_WIN)
void CalculateFileMd5(const UTF16String &file_path, UTF16String &md5_value)
{
	UTF8String utf8_md5_value;
	CalculateFileMd5(UTF16ToUTF8(file_path), utf8_md5_value);
	md5_value = std::move(UTF8ToUTF16(utf8_md5_value));
}
#endif
bool IsDirectoryComponent(const UTF8String &component)
{
	if (component.empty())
		return false;
	return *component.rbegin() == kFilePathSeparators[0] ||
		*component.rbegin() == kFilePathSeparators[1];
}
bool IsDirectoryComponent(const UTF16String &component)
{
	return IsDirectoryComponent(NS_EXTENSION::UTF16ToUTF8(component));
}

bool FilePathCompose(const UTF8String &directory_in,
					 const UTF8String &filename_in,
					 UTF8String &filepath_out)
{
	UTF8String directory;
	if (!FilePathApartDirectory(directory_in, directory))
		return false;
	filepath_out = directory + filename_in;
	return true;
}
#if defined(OS_WIN)
bool FilePathCompose(const UTF16String &directory_in,const UTF16String &filename_in,UTF16String &filepath_out)
{
	UTF8String out;
	if (FilePathCompose(NS_EXTENSION::UTF16ToUTF8(directory_in), NS_EXTENSION::UTF16ToUTF8(filename_in),out))
	{
		filepath_out = NS_EXTENSION::UTF8ToUTF16(out);
		return true;
	}
	return false;
}
#endif
bool FilePathIsExist(const UTF8String &filepath, bool is_directory)
{
	base::File::Info info;
	if (base::GetFileInfo(base::FilePath::FromUTF8Unsafe(filepath), &info))
		return is_directory ? info.is_directory : !info.is_directory;	
	return false;
}
#if defined(OS_WIN)
bool FilePathIsExist(const UTF16String &filepath_in, bool is_directory)
{
	return FilePathIsExist(NS_EXTENSION::UTF16ToUTF8(filepath_in), is_directory);
}
#endif
bool CreateDirectory(const UTF8String& full_path)
{
    return base::CreateDirectory(base::FilePath::FromUTF8Unsafe(full_path));
}
#if defined(OS_WIN)
bool CreateDirectory(const UTF16String& full_path)
{
	return CreateDirectory(NS_EXTENSION::UTF16ToUTF8(full_path));
}
#endif
FILE* OpenFile(const UTF8String &filepath, const char* mode)
{
	return base::OpenFile(base::FilePath::FromUTF8Unsafe(filepath), mode);
}
#if defined(OS_WIN)
FILE* OpenFile(const UTF16String &filepath, const char* mode)
{
	return OpenFile(NS_EXTENSION::UTF16ToUTF8(filepath), mode);
}
#endif
int ReadFile(const UTF8String &filepath, void *data_out, int size)
{
	return base::ReadFile(base::FilePath::FromUTF8Unsafe(filepath), static_cast<char*>(data_out), size);
}
#if defined(OS_WIN)
int ReadFile(const UTF16String &filepath, void *data_out, int size)
{
	return ReadFile(NS_EXTENSION::UTF16ToUTF8(filepath), data_out, size);
}
#endif
bool ReadFileToString(const UTF8String &filepath, std::string &contents_out)
{
	return base::ReadFileToString(NS_EXTENSION::FilePath::FromUTF8Unsafe(filepath), &contents_out);
}
#if defined(OS_WIN)
bool ReadFileToString(const UTF16String &filepath, std::string &contents_out)
{
	return ReadFileToString(NS_EXTENSION::UTF16ToUTF8(filepath), contents_out);
}
#endif
int WriteFile(const UTF8String &filepath, const std::string &data)
{
	return base::WriteFile(base::FilePath::FromUTF8Unsafe(filepath),data.data(),data.size());
}
#if defined(OS_WIN)
int WriteFile(const UTF16String &filepath, const std::string &data)
{
	return WriteFile(NS_EXTENSION::UTF16ToUTF8(filepath), data);
}
#endif
bool CloseFile(FILE* file)
{
	return base::CloseFile(file);
}
    
bool CopyFile(const UTF8String &from_path, const UTF8String &to_path)
{
	return base::CopyFile(base::FilePath::FromUTF8Unsafe(from_path), base::FilePath::FromUTF8Unsafe(to_path));
}
#if defined(OS_WIN)
bool CopyFile(const UTF16String &from_path, const UTF16String &to_path)
{
	return CopyFile(NS_EXTENSION::UTF16ToUTF8(from_path),NS_EXTENSION::UTF16ToUTF8(to_path));
}
#endif
// Moves a single file.
bool MoveFile(const UTF8String &from_path, const UTF8String &to_path)
{
	return base::Move(base::FilePath::FromUTF8Unsafe(from_path), base::FilePath::FromUTF8Unsafe(to_path));
}
#if defined(OS_WIN)
bool MoveFile(const UTF16String &from_path, const UTF16String &to_path)
{
	return MoveFile(NS_EXTENSION::UTF16ToUTF8(from_path),NS_EXTENSION::UTF16ToUTF8(to_path));
}
#endif
bool DeleteFile(const UTF8String &filepath)
{
	return base::DeleteFile(base::FilePath::FromUTF8Unsafe(filepath),(FilePathIsExist(filepath,true)?true:false));
}
#if defined(OS_WIN)
bool DeleteFile(const UTF16String &filepath)
{
	return DeleteFile(NS_EXTENSION::UTF16ToUTF8(filepath));
}
#endif
int64_t GetFileSize(const UTF8String &filepath)
{
	int64_t ret = 0;
	if (base::GetFileSize(base::FilePath::FromUTF8Unsafe(filepath),&ret))
        return ret;
	return 0;
}
#if defined(OS_WIN)
int64_t GetFileSize(const UTF16String &filepath)
{
	return GetFileSize(NS_EXTENSION::UTF16ToUTF8(filepath));
}
#endif
ExtensionFile ScopedFileTraits::InvalidValue() 
{ 
	return nullptr; 
}
void ScopedFileTraits::Free(ExtensionFile fd)
{
	CloseFile(fd);
}
ScopedFile::ScopedFile(const UTF8String& path, const UTF8String& open_flag) :
	ScopedFile::MyParent(OpenFile(path, open_flag.c_str()))
{
}
#if defined(OS_WIN)
ScopedFile::ScopedFile(const UTF16String& path, const UTF16String& open_flag) :
	ScopedFile::MyParent(OpenFile(path, UTF16ToUTF8(open_flag).c_str()))
{
}
#endif
FileEnumerator::FileEnumerator(const UTF8String& root_path, bool recursive, int file_type, const UTF8String& pattern) :
#if defined(OS_WIN)
	base::FileEnumerator(base::FilePath::FromUTF8Unsafe(root_path), recursive,file_type, UTF8ToUTF16(pattern))
#else
    base::FileEnumerator(base::FilePath::FromUTF8Unsafe(root_path), recursive,file_type, pattern)
#endif
{

}
EXTENSION_END_DECLS
