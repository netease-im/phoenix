#ifndef BASE_UTIL_STRING_UTIL_H_
#define BASE_UTIL_STRING_UTIL_H_

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <list>
#include "extension/strings/unicode.h"

EXTENSION_BEGIN_DECLS

// format a string
EXTENSION_EXPORT std::string StringPrintf(const char *format, ...);
#if defined(OS_WIN)
EXTENSION_EXPORT std::wstring StringPrintf(const wchar_t *format, ...);
#endif // _DEBUG
EXTENSION_EXPORT  std::string StringPrintf(std::string &output, const char *format, ...);
#if defined(OS_WIN)
EXTENSION_EXPORT   std::wstring StringPrintf(std::wstring &output, const wchar_t *format, ...);
#endif
EXTENSION_EXPORT void StringPrintfV(std::string &output, const char *format, va_list ap);
#if defined(OS_WIN)
EXTENSION_EXPORT void StringPrintfV(std::wstring &output, const wchar_t *format, va_list ap);
#endif // OS_WIN

// format a string and append the result to a existing string
EXTENSION_EXPORT void StringAppendF(std::string &output, const char *format, ...);
#if defined(OS_WIN)
EXTENSION_EXPORT void StringAppendF(std::wstring &output, const wchar_t *format, ...);
#endif // _DEBUG
EXTENSION_EXPORT void StringAppendV(std::string &output, const char *format, va_list ap);
#if defined(OS_WIN)
EXTENSION_EXPORT void StringAppendV(std::wstring &output, const wchar_t *format, va_list ap);
#endif // OS_WIN

// find all tokens splitted by one of the characters in 'delimitor'
EXTENSION_EXPORT std::list<UTF8String> StringTokenize(const char *input, const char *delimitor);
EXTENSION_EXPORT std::list<UTF16String> StringTokenize(const UTF16CharType *input, const UTF16CharType *delimitor);
EXTENSION_EXPORT int StringTokenize(const UTF8String& input, const UTF8String& delimitor, std::list<UTF8String>& output);
EXTENSION_EXPORT int StringTokenize(const UTF16String& input, const UTF16String& delimitor, std::list<UTF16String>& output);

// replace all 'find' with 'replace' in the string
EXTENSION_EXPORT size_t StringReplaceAll(const UTF8String& find, const UTF8String& replace, UTF8String& output);
EXTENSION_EXPORT size_t StringReplaceAll(const UTF16String& find, const UTF16String& replace, UTF16String& output);

// print the binary data in a human-readable hexadecimal format
EXTENSION_EXPORT void BinaryToHexString(const void *binary, size_t length, UTF8String &output);
EXTENSION_EXPORT UTF8String BinaryToHexString(const void *binary, size_t length);
EXTENSION_EXPORT UTF8String BinaryToHexString(const UTF8String &binary);

EXTENSION_EXPORT void LowerString(UTF8String &str);
EXTENSION_EXPORT void LowerString(UTF16String &str);
EXTENSION_EXPORT void UpperString(UTF8String &str);
EXTENSION_EXPORT void UpperString(UTF16String &str);
EXTENSION_EXPORT UTF8String MakeLowerString(const UTF8String &src);
EXTENSION_EXPORT UTF16String MakeLowerString(const UTF16String &src);
EXTENSION_EXPORT UTF8String MakeUpperString(const UTF8String &src);
EXTENSION_EXPORT UTF16String MakeUpperString(const UTF16String &src);

// convert hexadecimal string to binary
// the functions will try to convert the characters in input stream until the end of the stream or an error occurs
EXTENSION_EXPORT int HexStringToBinary(const char *input, size_t length, UTF8String &output);
EXTENSION_EXPORT UTF8String HexStringToBinary(const UTF8String &input);

// convert a hexadecimal character to decimal 8-bit signed integer
// if c is not a hexadecimal character, -1 will be returned
EXTENSION_EXPORT char HexCharToInt8(char c);

// the following functions are used to convert encodings in utf-8、utf-16 and utf-32
EXTENSION_EXPORT UTF16String UTF8ToUTF16(const UTF8CharType *utf8, size_t length);
EXTENSION_EXPORT UTF8String UTF16ToUTF8(const UTF16CharType *utf16, size_t length);
EXTENSION_EXPORT UTF16String UTF8ToUTF16(const UTF8String &utf8);
EXTENSION_EXPORT UTF8String UTF16ToUTF8(const UTF16String &utf16);
EXTENSION_EXPORT UTF16String ASCIIToUTF16(const std::string& ascii);
EXTENSION_EXPORT std::string UTF16ToASCII(const UTF16String& utf16);

#if defined(OS_WIN)
//多字节和宽字节转换
EXTENSION_EXPORT std::wstring MultiByteToWide(const std::string& mb, uint32_t code_page);
EXTENSION_EXPORT std::string WideToMultiByte(const std::wstring& wide,uint32_t code_page);

#endif  // defined(OS_WIN)

// the following functions are used to validate encoded streamings, such as utf-8, gbk, big5, etc.
EXTENSION_EXPORT void UTF8CreateLengthTable(unsigned table[256]);
EXTENSION_EXPORT bool ValidateUTF8Stream(const void* stream, unsigned length);
EXTENSION_EXPORT bool ValidateGB2312Stream(const void* stream, unsigned length);
EXTENSION_EXPORT bool ValidateGBKStream(const void* stream, unsigned length);
EXTENSION_EXPORT bool ValidateBIG5Stream(const void* stream, unsigned length);

// trimming, removing extra spaces
EXTENSION_EXPORT UTF8String StringTrimLeft(const char *input);
EXTENSION_EXPORT UTF8String StringTrimRight(const char *input);
EXTENSION_EXPORT UTF8String StringTrim(const char *input); /* both left and right */
EXTENSION_EXPORT UTF8String StringTrimLeft(const UTF8String &input);
EXTENSION_EXPORT UTF8String StringTrimRight(const UTF8String &input);
EXTENSION_EXPORT UTF8String StringTrim(const UTF8String &input); /* both left and right */
EXTENSION_EXPORT UTF16String StringTrimLeft(const UTF16CharType *input);
EXTENSION_EXPORT UTF16String StringTrimRight(const UTF16CharType *input);
EXTENSION_EXPORT UTF16String StringTrim(const UTF16CharType *input); /* both left and right */
EXTENSION_EXPORT UTF16String StringTrimLeft(const UTF16String &input);
EXTENSION_EXPORT UTF16String StringTrimRight(const UTF16String &input);
EXTENSION_EXPORT UTF16String StringTrim(const UTF16String &input); /* both left and right */

EXTENSION_END_DECLS

#endif // BASE_UTIL_STRING_UTIL_H_
