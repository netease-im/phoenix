#ifndef COMM_NIM_CRYPTO_EXPORT_H_
#define COMM_NIM_CRYPTO_EXPORT_H_

#if defined(COMPONENT_BUILD)
    #if defined(WIN32)
        #if defined(NIMCRYPTO_IMPLEMENTATION)
        #define NIMCRYPTO_EXPORT __declspec(dllexport)
        #define NIMCRYPTO_EXPORT_PRIVATE __declspec(dllexport)
        #else
        #define NIMCRYPTO_EXPORT __declspec(dllimport)
        #define NIMCRYPTO_EXPORT_PRIVATE __declspec(dllimport)
        #endif  // defined(NIMCRYPTO_IMPLEMENTATION)
    #else  // defined(WIN32)
        #if defined(NIMCRYPTO_IMPLEMENTATION)
            #define NIMCRYPTO_EXPORT __attribute__((visibility("default")))
            #define NIMCRYPTO_EXPORT_PRIVATE __attribute__((visibility("default")))
        #else
            #define HTTP_EXPORT
            #define HTTP_EXPORT_PRIVATE
        #endif  // defined(NET_IMPLEMENTATION)
    #endif
#else  // defined(COMPONENT_BUILD)
    #define HTTP_EXPORT
    #define HTTP_EXPORT_PRIVATE
#endif

#endif  // COMM_NIM_CRYPTO_EXPORT_H_
