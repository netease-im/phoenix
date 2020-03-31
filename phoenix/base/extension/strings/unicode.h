// Unicode define

#ifndef __BASE_EXTENSION_UTIL_UNICODE_H__
#define __BASE_EXTENSION_UTIL_UNICODE_H__

#include "build/build_config.h"
#include <string>

typedef char UTF8CharType;
#if defined(WCHAR_T_IS_UTF16)
typedef wchar_t UTF16CharType;
typedef int32_t UTF32CharType;
#elif defined(WCHAR_T_IS_UTF32)
typedef uint16_t UTF16CharType;
typedef wchar_t UTF32CharType;
#endif

typedef std::basic_string<UTF8CharType, std::char_traits<UTF8CharType>, std::allocator<UTF8CharType>> UTF8String;
typedef UTF8String U8String;
typedef std::basic_string<UTF16CharType, std::char_traits<UTF16CharType>, std::allocator<UTF16CharType>> UTF16String;
typedef UTF16String U16String;
typedef std::basic_string<UTF32CharType, std::char_traits<UTF32CharType>, std::allocator<UTF32CharType>> UTF32String;
typedef UTF32String U32String;

#endif // __BASE_EXTENSION_UTIL_UNICODE_H__
