
#ifndef BASE_HTTP_EXPORT_H_
#define BASE_HTTP_EXPORT_H_

#if defined(COMPONENT_BUILD)
    #if defined(WIN32)
        #if defined(HTTP_IMPLEMENTATION)
        #define HTTP_EXPORT __declspec(dllexport)
        #define HTTP_EXPORT_PRIVATE __declspec(dllexport)
        #else
        #define HTTP_EXPORT __declspec(dllimport)
        #define HTTP_EXPORT_PRIVATE __declspec(dllimport)
        #endif  // defined(NET_IMPLEMENTATION)
    #else  // defined(WIN32)
        #if defined(HTTP_IMPLEMENTATION)
            #define HTTP_EXPORT __attribute__((visibility("default")))
            #define HTTP_EXPORT_PRIVATE __attribute__((visibility("default")))
        #else
            #define HTTP_EXPORT
            #define HTTP_EXPORT_PRIVATE
        #endif  // defined(NET_IMPLEMENTATION)
    #endif
#else  // defined(COMPONENT_BUILD)
    #define HTTP_EXPORT
    #define HTTP_EXPORT_PRIVATE
#endif

#endif  // BASE_HTTP_EXPORT_H_
