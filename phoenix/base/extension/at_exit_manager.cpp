#include "extension/at_exit_manager.h"


EXTENSION_BEGIN_DECLS
AtExitManagerAdeptor::AtExitManager_Weak AtExitManagerAdeptor::at_exit_manager_;
NLock AtExitManagerAdeptor::at_exit_manager_lock_;
AtExitManager AtExitManagerAdeptor::GetAtExitManager()
{
	std::shared_ptr<base::AtExitManager> manager = at_exit_manager_.lock();
	if (manager == nullptr) {
		at_exit_manager_lock_.lock();
		manager = at_exit_manager_.lock();
		if (manager == nullptr)
		{
			manager.reset(new base::AtExitManager());
			at_exit_manager_ = manager;
		}		
		at_exit_manager_lock_.unlock();
	}
	return manager;
}
AtExitManager AtExitManagerAdeptor::TryGetAtExitManager()
{
	NAutoLock auto_lock(&at_exit_manager_lock_);
	return at_exit_manager_.lock();
}
EXTENSION_END_DECLS