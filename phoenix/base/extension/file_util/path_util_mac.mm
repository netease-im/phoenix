#include "extension/file_util/path_util.h"
#include "extension/file_util/path_util_mac.h"
#include "base/mac/foundation_util.h"
#include "base/files/file_path.h"

EXTENSION_BEGIN_DECLS


std::string GetSysLocalAppDataDirForMac()
{
	base::FilePath result;
	if (base::mac::GetUserDirectory(NSApplicationSupportDirectory, &result)) {
		base::FilePath  filePath = result.AsEndingWithSeparator();
		return filePath.value();
	}

	return "";
}
EXTENSION_END_DECLS
