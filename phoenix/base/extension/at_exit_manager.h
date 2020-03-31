#ifndef __EXTENSION_AT_EXIT_MANAGER_H__
#define __EXTENSION_AT_EXIT_MANAGER_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <memory>
#include "google_base/base/at_exit.h"
#include "extension/synchronization/lock.h"

EXTENSION_BEGIN_DECLS

using AtExitManager = std::shared_ptr<NS_BASE::AtExitManager>;
class EXTENSION_EXPORT AtExitManagerAdeptor
{
	using AtExitManager_Weak = std::weak_ptr<NS_BASE::AtExitManager>;
public:
	static AtExitManager GetAtExitManager();
	static AtExitManager TryGetAtExitManager();
private:
	static NLock at_exit_manager_lock_;
	static AtExitManager_Weak at_exit_manager_;
};

EXTENSION_END_DECLS
#endif//__EXTENSION_AT_EXIT_MANAGER_H__
