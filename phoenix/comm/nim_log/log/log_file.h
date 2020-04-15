#ifndef __BASE_EXTENSION_LOG_FILE_H__
#define __BASE_EXTENSION_LOG_FILE_H__
#include "nim_log/nim_log_export.h"
#include "nim_log/config/build_config.h"
#include <string>
#include <atomic>
#include <mutex>
#include <memory>

NIMLOG_BEGIN_DECLS

class NIMLOG_EXPORT LogFile
{
#if (defined OS_WIN) || (defined WIN32)
#define INVALID_LOG_FILE_HANDLE  nullptr
	using  LOG_FILE_HANDLE = void*;
#else
#define  INVALID_LOG_FILE_HANDLE (-1)
	using	LOG_FILE_HANDLE = int;
#endif
	class NIMLOG_EXPORT MMapFile
	{
	public:
		MMapFile();
		~MMapFile();
	public:
		bool Create(const std::string& log_path);
		bool Close();
		int UpdateCurrentLength(int length);
		bool Init();
		bool IsInited();
		bool Flush();
		bool Reset();
		int Write(const std::string& text);
		int Length();
		void AttachOverflowException(const std::function<bool(const std::string& text)>& callback)
		{
			overflow_callback_ = callback;
		}
	private:
		int Read(std::string& data);
		bool CheckMMapLogFile(const std::string& mmap_file_path,int max_length);
	private:
		const static int kMAX_LENGTH_;//最大的长度
		const static std::string kMMapFileExt_;
		std::recursive_mutex mutex_;
		bool inited_;
		int current_length_;//当前长度
		char* current_cursor_;//当前游标地址
		char* mapped_addr_;//映射的起始地址
		std::string file_path_;//被映射的文件
		LOG_FILE_HANDLE file_handle_;//被映射文件的句柄
		LOG_FILE_HANDLE mapped_file_handle_;//映射文件的句柄
		int data_offset_;//数据起始的偏移量
		std::function<bool(const std::string& text)> overflow_callback_;
	};
public:
	class OSFileSysUtil
	{
	public:
		static LOG_FILE_HANDLE CreateOSFile(const std::string& file_name, bool create, bool append = true, bool lock = false);
		static void CloseFile(LOG_FILE_HANDLE file);
		static bool IsFileExists(const std::string& file_path);
		static int64_t GetFileLength(LOG_FILE_HANDLE file);
		static bool MappingFile(LOG_FILE_HANDLE, int length, LOG_FILE_HANDLE& mapfile, char*& map_addr);
		static void UnMappingFile(LOG_FILE_HANDLE mapfile, void* map_addr, int size);
		static bool FlushMappingFile(LOG_FILE_HANDLE file);
	};
public:
	LogFile();
	~LogFile();
	bool Init(const std::string& log_file_path_);
	void WriteLog(const std::string& msg);
	bool Flush();
	void Close();
private:
	bool OnMappingFileOverflow(const std::string& text);
	bool ShrinkLogFile();
private:
	const static int kMAX_LOGFILE_LENGTH;//最大的长度
	std::unique_ptr< MMapFile> mmap_file_;
	std::string log_file_path_;
};

NIMLOG_END_DECLS

#endif//__BASE_EXTENSION_LOG_FILE_H__
