
#ifndef BASE_DB_EXPORT_H_
#define BASE_DB_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(DB_IMPLEMENTATION)
#define DB_EXPORT __declspec(dllexport)
#else
#define DB_EXPORT __declspec(dllimport)
#endif  // defined(DB_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(DB_IMPLEMENTATION)
#define DB_EXPORT __attribute__((visibility("default")))
#else
#define DB_EXPORT
#endif  // defined(DB_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define DB_EXPORT
#endif

#endif  // BASE_DB_EXPORT_H_
