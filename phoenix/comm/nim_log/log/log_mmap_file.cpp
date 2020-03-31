#include "extension/config/build_config.h"
#include "nim_log/log/log_file.h"

NIMLOG_BEGIN_DECLS

const int LogFile::MMapFile::kMAX_LENGTH_ = 64 * 1024;
const std::string LogFile::MMapFile::kMMapFileExt_ = ".nim_mmap";

LogFile::MMapFile::MMapFile() :
	inited_(false),
	current_length_(0),
	current_cursor_(nullptr),
	mapped_addr_(nullptr),
	file_handle_(INVALID_LOG_FILE_HANDLE),
	mapped_file_handle_(INVALID_LOG_FILE_HANDLE),
	data_offset_(0),
	overflow_callback_(nullptr)
{

}

LogFile::MMapFile::~MMapFile()
{

}

bool LogFile::MMapFile::Create(const std::string& log_path)
{
	file_path_ = log_path;
	file_path_.append(kMMapFileExt_);
	file_handle_ = LogFile::OSFileSysUtil::CreateOSFile(file_path_, true);
	if (file_handle_ == INVALID_LOG_FILE_HANDLE)
		return false;
	if (LogFile::OSFileSysUtil::MappingFile(file_handle_, kMAX_LENGTH_, mapped_file_handle_, mapped_addr_))
	{
		return Init();
	}
	else
	{
		return false;
	}
}

bool LogFile::MMapFile::Close()
{
	Flush();
	LogFile::OSFileSysUtil::UnMappingFile(mapped_file_handle_, mapped_addr_,kMAX_LENGTH_);
	LogFile::OSFileSysUtil::CloseFile(file_handle_);
	return true;
}

int LogFile::MMapFile::UpdateCurrentLength(int length)
{
	memcpy(mapped_addr_, &length, sizeof(length));
	return length;
}

bool LogFile::MMapFile::Init()
{
	if (inited_)
		return true;
	std::lock_guard<std::recursive_mutex> auto_lock(mutex_);
	if (inited_)
		return true;
	int len = Length();
	data_offset_ = sizeof(int);
	current_cursor_ = mapped_addr_ + len + data_offset_;
	current_length_ = len;
	if (len > 0 && overflow_callback_ != nullptr)
	{
		std::string text("\r\n -----------------------load from mmap file begin-----------------------\r\n");
		if (Read(text) == len)
		{
			text.append("\r\n -----------------------load from mmap file end-----------------------\r\n");
		}
		if (overflow_callback_(text))
			Reset();
	}
	inited_ = true;
	return inited_;
}

bool LogFile::MMapFile::IsInited()
{
	if (inited_)
		return true;
	std::lock_guard<std::recursive_mutex> auto_lock(mutex_);
	return inited_;
}

bool LogFile::MMapFile::Flush()
{
	if (IsInited())
	{
		std::string log_text;
		if (current_length_ > 0 && Read(log_text) == current_length_ && overflow_callback_ != nullptr)
		{
			if (overflow_callback_(log_text))
				Reset();
		}
		return true;
	}
	else
	{
		return Init();
	}

	return false;
}

bool LogFile::MMapFile::Reset()
{
	std::lock_guard<std::recursive_mutex> auto_lock(mutex_);
	current_cursor_ = mapped_addr_ + data_offset_;
	current_length_ = 0;
	memset(mapped_addr_, 0, kMAX_LENGTH_);
	UpdateCurrentLength(0);
	LogFile::OSFileSysUtil::FlushMappingFile(file_handle_);
	return true;
}

int LogFile::MMapFile::Write(const std::string& text)
{
	std::lock_guard<std::recursive_mutex> auto_lock(mutex_);
	int length = current_length_ + text.length() + sizeof(int);
	if (length >= kMAX_LENGTH_)
	{
		std::string log_text;
		if (Read(log_text) == current_length_)
		{
			log_text.append(text);
		}
		if (overflow_callback_(log_text))
			Reset();
	}
	else
	{
		current_length_ += text.length();
		memcpy(current_cursor_, text.c_str(), text.length());
		UpdateCurrentLength(current_length_);
		current_cursor_ += text.length();
		//LogFile::OSFileSysUtil::FlushMappingFile(file_handle_);
	}
	return 0;
}

int LogFile::MMapFile::Read(std::string& data)
{
	std::lock_guard<std::recursive_mutex> auto_lock(mutex_);
	data.append((mapped_addr_ + data_offset_), Length());
	return Length();
}

int LogFile::MMapFile::Length()
{
	std::lock_guard<std::recursive_mutex> auto_lock(mutex_);
	int length = 0;
	memcpy(&length, mapped_addr_, sizeof(length));
	return length;
}

NIMLOG_END_DECLS
