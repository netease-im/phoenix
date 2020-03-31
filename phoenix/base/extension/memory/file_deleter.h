#ifndef __BASE_EXTENSION_FILE_DELETER_H__
#define __BASE_EXTENSION_FILE_DELETER_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "base/files/file_util.h"

EXTENSION_BEGIN_DECLS

class EXTENSION_EXPORT DeleterFile {
public:
	inline void operator()(FILE * x) const {
		base::CloseFile(x);
	}
};


EXTENSION_END_DECLS


#endif // __BASE_EXTENSION_FILE_DELETER_H__
