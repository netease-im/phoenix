// String number conversion

#ifndef B__BASE_EXTENSION_STRING_NUMBER_CONVERSIONS_H__
#define B__BASE_EXTENSION_STRING_NUMBER_CONVERSIONS_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/strings/unicode.h"
#include "base/strings/string_number_conversions.h"

EXTENSION_BEGIN_DECLS

EXTENSION_EXPORT UTF8String IntToString(int value);
EXTENSION_EXPORT UTF16String IntToString16(int value);

EXTENSION_EXPORT UTF8String UintToString(unsigned int value);
EXTENSION_EXPORT UTF16String UintToString16(unsigned int value);

EXTENSION_EXPORT UTF8String Int64ToString(int64_t value);
EXTENSION_EXPORT UTF16String Int64ToString16(int64_t value);

EXTENSION_EXPORT UTF8String Uint64ToString(uint64_t value);
EXTENSION_EXPORT UTF16String Uint64ToString16(uint64_t value);

EXTENSION_EXPORT UTF8String DoubleToString(double value);

EXTENSION_EXPORT bool StringToInt(const UTF8String& input, int* output);
EXTENSION_EXPORT bool StringToInt(const UTF16String& input, int* output);

EXTENSION_EXPORT bool StringToUint(const UTF8String& input, unsigned* output);
EXTENSION_EXPORT bool StringToUint(const UTF16String& input, unsigned* output);

EXTENSION_EXPORT bool StringToInt64(const UTF8String& input, int64_t* output);
EXTENSION_EXPORT bool StringToInt64(const UTF16String& input, int64_t* output);

EXTENSION_EXPORT bool StringToUint64(const UTF8String& input, uint64_t* output);
EXTENSION_EXPORT bool StringToUint64(const UTF16String& input, uint64_t* output);

EXTENSION_EXPORT bool StringToSizeT(const UTF8String& input, size_t* output);
EXTENSION_EXPORT bool StringToSizeT(const UTF16String& input, size_t* output);

EXTENSION_EXPORT bool StringToDouble(const UTF8String& input, double* output);

EXTENSION_EXPORT bool HexStringToInt(const UTF8String& input, int* output);

EXTENSION_END_DECLS

#endif  // B__BASE_EXTENSION_STRING_NUMBER_CONVERSIONS_H__

