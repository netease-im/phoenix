
#ifndef BASE_EXTENSION_EXPORT_H_
#define BASE_EXTENSION_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(EXTENSION_IMPLEMENTATION)
#define EXTENSION_EXPORT __declspec(dllexport)
#else
#define EXTENSION_EXPORT __declspec(dllimport)
#endif  // defined(EXTENSION_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(EXTENSION_IMPLEMENTATION)
#define EXTENSION_EXPORT __attribute__((visibility("default")))
#else
#define EXTENSION_EXPORT
#endif  // defined(EXTENSION_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define EXTENSION_EXPORT
#endif

#endif  // BASE_EXTENSION_EXPORT_H_
