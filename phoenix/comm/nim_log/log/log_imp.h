#ifndef __BASE_EXTENSION_LOGIMP_H__
#define __BASE_EXTENSION_LOGIMP_H__
#include "nim_log/config/build_config.h"
#include <string>
#include "nim_log/log/log_def.h"
#include "nim_log/log/log_file.h"

NIMLOG_BEGIN_DECLS

class LogMessageImpl;
class NIMLOG_EXPORT QLogImpl : public ILogger, public ILogWriter<LogMessageImpl>
{
public:
	QLogImpl();
public:
	virtual void SetLogFile(const std::string &file_path) override;
	virtual std::string GetLogFile() override;
	virtual void SetLogLevel(LOG_LEVEL lv) override;
	virtual LOG_LEVEL GetLogLevel() override { return log_level_; }
	virtual bool Flush() override;
	virtual void Release() override;
public:
	void WriteLog(LOG_LEVEL lv, const std::string &log);
private:
	std::unique_ptr<LogFile> instance_;
	std::string log_file_;
	LOG_LEVEL	 log_level_;
};
class NIMLOG_EXPORT LogMessageImpl : public ILogMessage
{
public:
	LogMessageImpl(const char* file, long line,const Logger& writer);
	virtual ~LogMessageImpl();
	std::string TLog();
	virtual ILogMessage& VLog(LOG_LEVEL lv, const std::string &fmt) override; 
	virtual ILogMessage& operator<<(const std::string &str)  override;
	virtual ILogMessage& operator<<(const char* temp)  override;
	virtual ILogMessage& operator<<(const bool temp)  override;
	virtual ILogMessage& operator<<(const int8_t temp)  override;
	virtual ILogMessage& operator<<(const uint8_t temp)  override;
	virtual ILogMessage& operator<<(const int16_t temp)  override;
	virtual ILogMessage& operator<<(const uint16_t temp)  override;
	virtual ILogMessage& operator<<(const int32_t temp)  override;
	virtual ILogMessage& operator<<(const uint32_t temp)  override;
	virtual ILogMessage& operator<<(const int64_t temp)  override;
	virtual ILogMessage& operator<<(const uint64_t temp)  override;
	virtual ILogMessage& operator<<(const long temp)  override;
	virtual ILogMessage& operator<<(const unsigned long temp)  override;
	virtual ILogMessage& operator<<(const double temp)  override;
private:
	size_t FindArg(int &len); //返回"{x}"的索引，len代表"{x}"的长度
private:
	LogWriter<LogMessageImpl> writer_;
	std::string fmt_;
	std::string string_;
	int			 index_;
	std::string time_;
	std::string file_line_;
	std::string process_thread_;
	LOG_LEVEL	 level_;
	std::string lv_;
};
NIMLOG_END_DECLS

#endif
