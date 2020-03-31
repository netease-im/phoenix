#include "extension/strings/string_number_conversions.h"

EXTENSION_BEGIN_DECLS

UTF8String IntToString(int value) {
  return base::Int64ToString(value);
}

UTF16String IntToString16(int value) {
  return base::IntToString16(value).c_str();
}

UTF8String UintToString(unsigned int value) {
  return base::UintToString(value);
}
UTF16String UintToString16(unsigned int value) {
  return base::UintToString16(value).c_str();
}

UTF8String Int64ToString(int64_t value) {
  return base::Int64ToString(value);
}

UTF16String Int64ToString16(int64_t value) {
  return base::Int64ToString16(value).c_str();
}

UTF8String Uint64ToString(uint64_t value) {
  return base::Uint64ToString(value);
}

UTF16String Uint64ToString16(uint64_t value) {
  return base::Uint64ToString16(value).c_str();
}

UTF8String DoubleToString(double value) {
  return base::DoubleToString(value);
}

bool StringToInt(const UTF8String& input, int* output) {
  return base::StringToInt(input,output);
}

bool StringToInt(const UTF16String& input, int* output) {
  return base::StringToInt(input.c_str(),output);
}

bool StringToUint(const UTF8String& input, unsigned* output) {
  return base::StringToUint(input,output);
}

bool StringToUint(const UTF16String& input, unsigned* output) {
  return base::StringToUint(input.c_str(),output);
}

bool StringToInt64(const UTF8String& input, int64_t* output) {
  return base::StringToInt64(input,output);
}

bool StringToInt64(const UTF16String& input, int64_t* output) {
  return base::StringToInt64(input.c_str(),output);
}

bool StringToUint64(const UTF8String& input, uint64_t* output) {
  return base::StringToUint64(input,output);
}

bool StringToUint64(const UTF16String& input, uint64_t* output) {
  return base::StringToUint64(input.c_str(),output);
}

bool StringToSizeT(const UTF8String& input, size_t* output) {
  return base::StringToSizeT(input,output);
}

bool StringToSizeT(const UTF16String& input, size_t* output) {
  return base::StringToSizeT(input.c_str(),output);
}

bool StringToDouble(const UTF8String& input, double* output) {
	return base::StringToDouble(input,output);
}
bool HexStringToInt(const UTF8String& input, int* output) {
	return base::HexStringToInt(input,output);
}

EXTENSION_END_DECLS
