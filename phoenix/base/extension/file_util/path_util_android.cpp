#include "extension/file_util/path_util.h"
#include "extension/file_util/path_util_android.h"
#include "base/android/path_utils.h"
#include "base/files/file_path.h"

EXTENSION_BEGIN_DECLS


std::string GetSysLocalAppDataDirForAndroid()
{
	base::FilePath result;
	if (base::android::GetExternalStorageDirectory(&result)) {
		base::FilePath  filePath = result.AsEndingWithSeparator();
		return filePath.value();
	}

	return "";
}
EXTENSION_END_DECLS
