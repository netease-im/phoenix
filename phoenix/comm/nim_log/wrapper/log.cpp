#include "nim_log/wrapper/log.h"
#include "nim_log/log/log_imp.h"
NIMLOG_BEGIN_DECLS
Logger NIMLog::CreateLogger()
{
	return std::make_shared<QLogImpl>();
}
LogMessage NIMLog::CreateLogMessage(const char* file, long line, const Logger& logger)
{
	return std::make_unique<LogMessageImpl>(file, line, logger);
}
NIMLOG_END_DECLS