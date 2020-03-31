// This file defines some useful macros

#ifndef __BASE_EXTENSION_MACROS_H__
#define __BASE_EXTENSION_MACROS_H__

#include <assert.h>
#include <string>
//#include "base_types.h"

// check the pointer is null and return null
#define PTR_0(p)				{ assert(p); if (0 == p) return 0; }
// check the pointer is null and return void
#define PTR_VOID(p)				{ assert(p); if (0 == p) return; }
// check the pointer avalible， if not then return false
#define PTR_FALSE(p)			{ assert(p); if (0 == p) return false; }

// delete pointer on safe mode
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if (0 != p) { delete (p); (p) = 0; } }
#endif

// release object on safe mode
#define SAFE_RELEASE(p)			{ if (0 != p) { (p)->Release(); (p) = 0; } }

// //namespace
// #define NAMESPACE_USING(n)		using namespace n;
// #define NAMESPACE_BEGIN(n)		namespace n {
// #define NAMESPACE_END(n)		}

#ifndef COUNT_OF
#define COUNT_OF(array)			(sizeof(array)/sizeof(array[0]))
#endif

// define some format string
#define CHAR_FORMAT                             "%c"
#define POINTER_FORMAT                          "%p"
#define STRING_FORMAT                           "%s"

#define INT8_FORMAT                             "%d"
#define UINT8_FORMAT                            "%u"
#define INT16_FORMAT                            "%d"
#define UINT16_FORMAT                           "%u"
#define INT32_FORMAT                            "%d"
#define UINT32_FORMAT                           "%u"

#define HEX8_FORMAT                             "%x"
#define HEX16_FORMAT                            "%x"
#define HEX32_FORMAT                            "%x"

#define OCT8_FORMAT                             "%o"
#define OCT16_FORMAT                            "%o"
#define OCT32_FORMAT                            "%o"

#define FLOAT_FORMAT                            "%f"
#define DOUBLE_FORMAT                           "%lf"

#if defined(COMPILER_MSVC)
#define INT64_FORMAT                            "%I64d"
#define UINT64_FORMAT                           "%I64u"
#define HEX64_FORMAT                            "%I64x"
#define OCT64_FORMAT                            "%I64o"
#else  //OS_WIN
#define UINT64_FORMAT                           "%llu"
#define INT64_FORMAT                            "%lld"
#define HEX64_FORMAT                            "%llx"
#define OCT64_FORMAT                            "%llo"
#endif //OS_WIN

#endif  // __BASE_EXTENSION_MACROS_H__
