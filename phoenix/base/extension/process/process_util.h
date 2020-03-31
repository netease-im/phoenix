#ifndef __BASE_EXTENSION_PROCESS_H__
#define __BASE_EXTENSION_PROCESS_H__
#include "extension/config/build_config.h"
#include "base/process/process.h"
#include "base/threading/platform_thread.h"

EXTENSION_BEGIN_DECLS

using Process = base::Process;
using PlatformThread = base::PlatformThread;

EXTENSION_END_DECLS
#endif // !__BASE_EXTENSION_PROCESS_H__
