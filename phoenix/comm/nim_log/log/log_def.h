#ifndef __BASE_EXTENSION_LOGDEF_H__
#define __BASE_EXTENSION_LOGDEF_H__
#include "nim_log/config/build_config.h"
#include "nim_log/nim_log_export.h"
#include <memory>
#include <string>
NIMLOG_BEGIN_DECLS

enum LOG_LEVEL
{
	LV_KER = 0,
	LV_ASS = 1,
	LV_ERR = 2,
	LV_WAR = 3,
	LV_INT = 4,
	LV_APP = 5,
	LV_PRO = 6
};
class NIMLOG_EXPORT ILogMessage
{
public:
	ILogMessage() = default;
	virtual ~ILogMessage() = default;
public:
	virtual ILogMessage& VLog(LOG_LEVEL lv, const std::string &fmt) = 0;
	virtual ILogMessage& operator<<(const std::string &str) = 0;
	virtual ILogMessage& operator<<(const char* temp) = 0;
	virtual ILogMessage& operator<<(const bool temp) = 0;
	virtual ILogMessage& operator<<(const int8_t temp) = 0;
	virtual ILogMessage& operator<<(const uint8_t temp) = 0;
	virtual ILogMessage& operator<<(const int16_t temp) = 0;
	virtual ILogMessage& operator<<(const uint16_t temp) = 0;
	virtual ILogMessage& operator<<(const int32_t temp) = 0;
	virtual ILogMessage& operator<<(const uint32_t temp) = 0;
	virtual ILogMessage& operator<<(const int64_t temp) = 0;
	virtual ILogMessage& operator<<(const uint64_t temp) = 0;
	virtual ILogMessage& operator<<(const long temp) = 0;
	virtual ILogMessage& operator<<(const unsigned long temp) = 0;
	virtual ILogMessage& operator<<(const double temp) = 0;
};
using LogMessage = std::unique_ptr<ILogMessage>;
class NIMLOG_EXPORT ILogger
{
public:
	virtual ~ILogger() = default;
public:
	virtual void SetLogFile(const std::string &file_path) = 0;
	virtual std::string GetLogFile() = 0;
	virtual void SetLogLevel(LOG_LEVEL lv) = 0;
	virtual LOG_LEVEL GetLogLevel() = 0;
	virtual bool Flush() = 0;
	virtual void Release() = 0;
};
using Logger = std::shared_ptr<ILogger>;
template<class T>
class NIMLOG_EXPORT ILogWriter
{
	using __FriendClass = typename T;
	friend __FriendClass;
public:
	virtual ~ILogWriter() = default;
protected:
	virtual void WriteLog(LOG_LEVEL lv, const std::string &log) = 0;
};
template<typename T>
using LogWriter = std::shared_ptr<ILogWriter<T>>;
class ILoggerSetter
{
public:
	virtual void SetLogger(const Logger& logger) = 0;
	virtual Logger GetLogger() const = 0;
protected:
	virtual void OnSetLogger() = 0;
};
NIMLOG_END_DECLS
#endif