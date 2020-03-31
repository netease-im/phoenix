#include "extension/log/log_file.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
namespace nbase
{
	LogFile::LOG_FILE_HANDLE LogFile::OSFileSysUtil::CreateOSFile(const std::string& file_path, bool create, bool append /*= true*/, bool lock/* = false*/)
	{
		LogFile::LOG_FILE_HANDLE ret = INVALID_LOG_FILE_HANDLE;
		// Set mode
		int oflag = O_RDWR;

		if (create)
			oflag |= O_CREAT;

		if (append)
			oflag |= O_APPEND;	
		// Open file descriptor
		ret = open(file_path.c_str(), oflag, S_IRWXU);

		if (ret == -1)
		{
			// Reinitialize fd
			ret = INVALID_LOG_FILE_HANDLE;
		}

		return ret;
	}
	void LogFile::OSFileSysUtil::CloseFile(LogFile::LOG_FILE_HANDLE file)
	{
		close(file);
	}
	bool LogFile::OSFileSysUtil::IsFileExists(const std::string& file_path)
	{
		static struct stat stats;
		int ret = stat(file_path.c_str(), &stats);
		if (ret == 0 && S_ISREG(stats.st_mode))
			return true;

		return false;
	}
	int64_t LogFile::OSFileSysUtil::GetFileLength(LogFile::LOG_FILE_HANDLE file)
	{
		if (file != -1)
		{
			// Get current position
			off_t position = lseek(file, 0, SEEK_CUR);

			// Get length
			off_t length = lseek(file, 0, SEEK_END);

			// Move back
			lseek(file, position, SEEK_SET);

			return length;
		}

		return 0;
	}
	bool LogFile::OSFileSysUtil::MappingFile(LogFile::LOG_FILE_HANDLE file, int length, LogFile::LOG_FILE_HANDLE& mapfile, char*& map_addr)
	{
		mapfile = INVALID_LOG_FILE_HANDLE;
		map_addr = nullptr;
		int file_length = GetFileLength(file);
		if (file_length < length)
		{
			ftruncate(file, length);
			}
		map_addr = (char*)mmap(NULL, length, PROT_WRITE, MAP_SHARED, file, 0);
		if (map_addr == MAP_FAILED)
		{
			map_addr = nullptr;
			return false;
		}
		return true;
	}
	void LogFile::OSFileSysUtil::UnMappingFile(LogFile::LOG_FILE_HANDLE mapfile, void* map_addr, int size)
	{
		if (map_addr != nullptr)
		{
			munmap(map_addr,size);
		}
	}
	bool LogFile::OSFileSysUtil::FlushMappingFile(LogFile::LOG_FILE_HANDLE file)
	{
	/**see http://man7.org/linux/man-pages/man2/fdatasync.2.html
	* fdatasync() is similar to fsync(), but does not flush modified
	* metadata unless that metadata is needed in order to allow a
	* subsequent data retrieval to be correctly handled.  For example,
	* changes to st_atime or st_mtime (respectively, time of last access
	* and time of last modification; see stat(2)) do not require flushing
	* because they are not necessary for a subsequent data read to be
	* handled correctly.  On the other hand, a change to the file size
	* (st_size, as made by say ftruncate(2)), would require a metadata
	* flush.
	* The aim of fdatasync() is to reduce disk activity for applications
	* that do not require all metadata to be synchronized with the disk.
	*/
		if (file != -1)
		{	
#ifdef __APPLE__
	int ret = fsync(file);
#else
	int ret = fdatasync(file);
#endif
			return ret == 0;
		}
		return false;
	}
};//nbase
