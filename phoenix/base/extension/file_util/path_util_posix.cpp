#include "extension/file_util/path_util.h"
#include "extension/file_util/path_util_posix.h"
#include "base/files/file_path.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h> 

EXTENSION_BEGIN_DECLS


std::string GetSysLocalAppDataDirForPosix()
{
	std::string home_dir_path = getenv("HOME");
	if (!home_dir_path.empty())
	{
		FilePath filePath = base::FilePath::FromUTF8Unsafe(home_dir_path).AsEndingWithSeparator();
		return filePath.value();
	}
	return "";
}
EXTENSION_END_DECLS
