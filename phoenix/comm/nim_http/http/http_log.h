#ifndef NETWORK_HTTP_HTTP_HTTP_LOG_H_
#define NETWORK_HTTP_HTTP_HTTP_LOG_H_
#include "nim_log/log/log_def.h"

#define HTTP_QLOG_PRO(Logger,fmt) __NIM_LOG_PRO(fmt,Logger)
#define HTTP_QLOG_APP(Logger,fmt) __NIM_LOG_APP(fmt,Logger)
#define HTTP_QLOG_WAR(Logger,fmt) __NIM_LOG_WAR(fmt,Logger)
#define HTTP_QLOG_ERR(Logger,fmt) __NIM_LOG_ERR(fmt,Logger)
#define HTTP_QLOG_KER(Logger,fmt) __NIM_LOG_KER(fmt,Logger)
#define HTTP_QLOG_ASS(Logger,fmt) __NIM_LOG_ASS(fmt,Logger)
#define HTTP_QLOG_INT(Logger,fmt) __NIM_LOG_INT(fmt,Logger)
#endif
