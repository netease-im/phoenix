
#ifndef COMM_NIMLOG_EXPORT_H_
#define COMM_NIMLOG_EXPORT_H_

#if defined(COMPONENT_BUILD)
    #if defined(WIN32)
        #if defined(NIMLOG_IMPLEMENTATION)
        #define NIMLOG_EXPORT __declspec(dllexport)
        #define NIMLOG_EXPORT_PRIVATE __declspec(dllexport)
        #else
        #define NIMLOG_EXPORT __declspec(dllimport)
        #define NIMLOG_EXPORT_PRIVATE __declspec(dllimport)
        #endif  // defined(NIMLOG_IMPLEMENTATION)
    #else  // defined(WIN32)
        #if defined(NIMLOG_IMPLEMENTATION)
            #define NIMLOG_EXPORT __attribute__((visibility("default")))
            #define NIMLOG_EXPORT_PRIVATE __attribute__((visibility("default")))
        #else
            #define NIMLOG_EXPORT
            #define NIMLOG_EXPORT_PRIVATE
        #endif  // defined(NET_IMPLEMENTATION)
    #endif
#else  // defined(COMPONENT_BUILD)
    #define NIMLOG_EXPORT
    #define NIMLOG_EXPORT_PRIVATE
#endif

#endif  // COMM_NIMLOG_EXPORT_H_
