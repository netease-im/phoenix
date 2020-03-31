#include "nim_log/log/log_file.h"
#include "nim_log/config/build_config.h"
#include <fstream>
#include "extension/file_util/utf8_file_util.h"
#include "extension/strings/string_util.h"
#include "nim_log/config/build_config.h"
#include "nim_log/log/log_file.h"


NIMLOG_BEGIN_DECLS

LogFile::LogFile() :
	mmap_file_(nullptr)
{
}
LogFile::~LogFile()
{

}
const int LogFile::kMAX_LOGFILE_LENGTH = 8 * 1024*1024;
bool LogFile::Init(const std::string& log_file_path)
{
	log_file_path_ = log_file_path;
	if (mmap_file_ == nullptr)
	{
		mmap_file_ = std::make_unique<MMapFile>();
		mmap_file_->AttachOverflowException(std::bind(&LogFile::OnMappingFileOverflow, this, std::placeholders::_1));
		mmap_file_->Create(log_file_path_);
	}
	if (!mmap_file_->IsInited())
		mmap_file_->Init();
	return mmap_file_->IsInited();
}
void LogFile::WriteLog(const std::string& msg)
{
	mmap_file_->Write(msg);
}

bool LogFile::Flush()
{
	return mmap_file_->Flush();
}

bool LogFile::OnMappingFileOverflow(const std::string& text)
{
	std::ofstream log_file(log_file_path_, std::ios::app | std::ios::out);
	if (log_file.is_open())
	{
		log_file << text;
		log_file.close();
	}
	return ShrinkLogFile();
}
bool LogFile::ShrinkLogFile()
{
	bool ret = true;		
	std::fstream fin(log_file_path_, std::ios::in | std::ios::out /*| std::ios::_Nocreate*/);
		
	if (fin.is_open() )
	{
		fin.seekg(0, std::ios::end);                      // 设置指针到文件流尾部
		int len = fin.tellg();                  // 指针距离文件头部的距离，即为文件流大小		
		if (len >= kMAX_LOGFILE_LENGTH)
		{
			std::string temp(log_file_path_);
			temp.append(".tmp");
			std::fstream fout(temp, std::ios::out | std::ios::trunc);
			if (fout.is_open())
			{
				fin.seekg(0, std::ios::beg);
				fin.seekg(-kMAX_LOGFILE_LENGTH / 2, std::ios::end);
				char buffer[1024 * 8];
				static const int buffer_legth = 1024 * 8;
				memset(buffer, 0, buffer_legth * sizeof(char));
				while (!fin.eof())
				{
					fin.read(buffer, buffer_legth);
					fout << buffer;
					memset(buffer, 0, buffer_legth * sizeof(char));
				}
				fout.close();
				fin.close();
				NS_EXTENSION::DeleteFile(log_file_path_);
				NS_EXTENSION::MoveFile(temp,log_file_path_);
			}
		}
		if(fin.is_open())
			fin.close();
	}
	return ret;
}
void LogFile::Close()
{
	mmap_file_->Close();
	mmap_file_.release();
}

NIMLOG_END_DECLS

