#pragma once
#include <Windows.h>
#include "nim_log/wrapper/log.h"
#include "extension/memory/singleton.h"

USING_NS_NIMLOG
class TestLogger : public NS_EXTENSION::Singleton<TestLogger,false>
{
	SingletonHideConstructor(TestLogger);
private:
	TestLogger() :logger_(NIMLog::CreateLogger()) {}
	~TestLogger() = default;
public:
	inline const Logger& GetLogger() const {
		return logger_;
	}
private:
	Logger logger_;
};
#define TEST_LOG_PRO(fmt) __NIM_LOG_PRO(fmt,TestLogger::GetInstance()->GetLogger())
#define TEST_LOG_APP(fmt) __NIM_LOG_APP(fmt,TestLogger::GetInstance()->GetLogger())
#define TEST_LOG_WAR(fmt) __NIM_LOG_WAR(fmt,TestLogger::GetInstance()->GetLogger())
#define TEST_LOG_ERR(fmt) __NIM_LOG_ERR(fmt,TestLogger::GetInstance()->GetLogger())
#define TEST_LOG_KER(fmt) __NIM_LOG_KER(fmt,TestLogger::GetInstance()->GetLogger())
#define TEST_LOG_ASS(fmt) __NIM_LOG_ASS(fmt,TestLogger::GetInstance()->GetLogger())
#define TEST_LOG_INT(fmt) __NIM_LOG_INT(fmt,TestLogger::GetInstance()->GetLogger())


class LogTestItem
{
public:
	void DoSomething()
	{
		DWORD dwErr, systemLocale = 0;
		HLOCAL hLocal = NULL;
		dwErr = ::GetLastError();
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL, dwErr, systemLocale, (LPSTR)&hLocal, 0, NULL);
		TEST_LOG_ERR("[LogTestItem] LastError :{0} Error Message:{1}") << dwErr << (LPSTR)hLocal;
	}
};
class LogTest 
{
public:
	void Init()
	{
		DWORD dwErr, systemLocale = 0;
		HLOCAL hLocal = NULL;
		dwErr = ::GetLastError();		
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL, dwErr, systemLocale, (LPSTR)&hLocal, 0, NULL);
		TEST_LOG_ERR( "[LogTest] LastError :{0} Error Message:{1}") << dwErr << (LPSTR)hLocal;
		item.DoSomething();
	}
private:
	LogTestItem item;
};
