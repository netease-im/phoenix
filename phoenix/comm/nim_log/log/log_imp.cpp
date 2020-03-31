#include "nim_log/log/log_imp.h"
#include <cassert>
#include <map>
#ifdef _DEBUG
#include <iostream>
#endif
#include "extension/time/time.h"
#include "extension/strings/string_util.h"
#include "extension/process/process_util.h"

NIMLOG_BEGIN_DECLS

QLogImpl::QLogImpl() :
	instance_(std::make_unique<LogFile>())
{

}
void QLogImpl::SetLogFile(const std::string &file_path)
{
	log_file_ = file_path;
	instance_->Init(file_path);
}

std::string QLogImpl::GetLogFile()
{
	return log_file_;
}

bool QLogImpl::Flush()
{
	return instance_->Flush();
}

void QLogImpl::Release()
{
	instance_->Close();
}

void QLogImpl::SetLogLevel(LOG_LEVEL lv)
{
	log_level_ = lv;
}

void QLogImpl::WriteLog(LOG_LEVEL lv, const std::string &log)
{
#ifdef _DEBUG
	std::cout << log.c_str() << std::endl;
#endif
	if (lv > log_level_)
		return;
	if (log_file_.empty())
		return;
	instance_->WriteLog(log);
}

LogMessageImpl::LogMessageImpl(const char* file, long line, const Logger& writer) :
	writer_(std::dynamic_pointer_cast<QLogImpl>(writer)), index_(0), level_(LV_PRO)
{
	NS_EXTENSION::TimeStruct qt;
	NS_EXTENSION::Time::Now().LocalExplode(&qt);
	time_ = NS_EXTENSION::StringPrintf("%02d-%02d %02d:%02d:%02d.%03d",
		qt.month, qt.day_of_month, qt.hour, qt.minute, qt.second, qt.millisecond);
#if !defined(OS_IOS)
	process_thread_.append(std::to_string(NS_EXTENSION::Process::Current().Pid())).append("-").
		append(std::to_string(NS_EXTENSION::PlatformThread::CurrentId()));
#endif
}
std::string LogMessageImpl::TLog()
{
	static const char log_format[] = "[%s %s] [%s] %s \n";
	std::string log_text;
	if (string_.empty())
		string_ = fmt_;
	else if (!fmt_.empty())
		string_.append(fmt_);
	return NS_EXTENSION::StringPrintf(log_text, log_format, time_.c_str(), process_thread_.c_str(), lv_.c_str(), string_.c_str());
}

LogMessageImpl::~LogMessageImpl()
{
	if(writer_ != nullptr)
		writer_->WriteLog(level_, TLog());
}

ILogMessage& LogMessageImpl::VLog(LOG_LEVEL lv, const std::string &fmt)
{
	static std::map< LOG_LEVEL, std::string> kLEVEL_TEXT_LIST = {
		{LV_KER,"LV_KER"},	{LV_ASS,"LV_ASS"},{LV_ERR,"LV_ERR"},
		{LV_WAR,"LV_WAR"},{LV_INT,"LV_INT"},{LV_APP,"LV_APP"},
		{LV_PRO,"LV_PRO"}
	};
	level_ = lv;
	fmt_ = fmt;
	lv_.append(kLEVEL_TEXT_LIST[level_]);
	return *((ILogMessage*)this);
}

ILogMessage& LogMessageImpl::operator<<(const std::string &str)
{
	int len = 0;
	size_t pos = FindArg(len);
	if (pos == std::string::npos)
	{
		assert(0);
	}
	else
	{
		string_.append(fmt_.substr(0, pos));
		string_.append(str);
		fmt_.erase(0, pos + len);
	}
	index_++;
	return *((ILogMessage*)this);
}

ILogMessage&LogMessageImpl::operator<<(const char* temp)
{
	return this->operator<<(std::string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const bool temp)
{
	return this->operator<<(std::string(temp ? "true" : "false"));
}

ILogMessage& LogMessageImpl::operator<<(const int8_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const uint8_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const int16_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const uint16_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const int32_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const uint32_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const int64_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const uint64_t temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const long temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const unsigned long temp)
{
	return this->operator<<(std::to_string(temp));
}

ILogMessage& LogMessageImpl::operator<<(const double db)
{
	return this->operator<<(std::to_string(db));
}

size_t LogMessageImpl::FindArg(int &len)
{
	assert(!fmt_.empty());
	assert(index_ >= 0 && index_ <= 20);

	std::string str("{");
	str.append(std::to_string(index_)).append("}");
	len = str.length();
	return fmt_.find(str);
}
NIMLOG_END_DECLS