#ifndef __BASE_EXTENSION_LOG_H__
#define __BASE_EXTENSION_LOG_H__
#include "nim_log/config/build_config.h"
#include "nim_log/log/log_def.h"

NIMLOG_BEGIN_DECLS

class NIMLog
{
private:
	NIMLog() = default;
	~NIMLog() = default;
public:
	static Logger CreateLogger();
	static LogMessage CreateLogMessage(const char* file, long line, const Logger& logger);
};

class LoggerSetter : public virtual ILoggerSetter
{
public:
	LoggerSetter() : logger_(nullptr) {}
	virtual ~LoggerSetter() = default;
public:
	virtual void SetLogger(const Logger& logger) override { logger_ = logger; OnSetLogger(); };
	virtual Logger GetLogger() const { return logger_; };
protected:
	virtual void OnSetLogger() override {};
protected:
	Logger logger_;
};
NIMLOG_END_DECLS

#define __NIM_LOG_PRO(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_PRO, fmt)
#define __NIM_LOG_APP(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_APP, fmt)
#define __NIM_LOG_WAR(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_WAR, fmt)
#define __NIM_LOG_ERR(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_ERR, fmt)
#define __NIM_LOG_KER(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_KER, fmt)
#define __NIM_LOG_ASS(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_ASS, fmt)
#define __NIM_LOG_INT(fmt,Logger) NS_NIMLOG::NIMLog::CreateLogMessage(__FILE__, __LINE__,Logger)->VLog(NS_NIMLOG::LOG_LEVEL::LV_INT, fmt)

#endif//__BASE_EXTENSION_LOG_H__