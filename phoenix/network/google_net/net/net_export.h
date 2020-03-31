
#ifndef NETWORK_GOOGLE_NET_NET_EXPORT_H_
#define NETWORK_GOOGLE_NET_NET_EXPORT_H_

#if defined(COMPONENT_BUILD)
    #if defined(WIN32)
        #if defined(NET_IMPLEMENTATION)
        #define NET_EXPORT __declspec(dllexport)
        #define NET_EXPORT_PRIVATE __declspec(dllexport)
        #else
        #define NET_EXPORT __declspec(dllimport)
        #define NET_EXPORT_PRIVATE __declspec(dllimport)
        #endif  // defined(NET_IMPLEMENTATION)
    #else  // defined(WIN32)
        #if defined(NET_IMPLEMENTATION)
            #define NET_EXPORT __attribute__((visibility("default")))
            #define NET_EXPORT_PRIVATE __attribute__((visibility("default")))
        #else
            #define NET_EXPORT
            #define NET_EXPORT_PRIVATE
        #endif  // defined(NET_IMPLEMENTATION)
    #endif
#else  // defined(COMPONENT_BUILD)
    #define NET_EXPORT
    #define NET_EXPORT_PRIVATE
#endif

#endif  // NETWORK_GOOGLE_NET_NET_EXPORT_H_
