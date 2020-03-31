#ifndef __BASE_EXTENSION_LOCK_H__
#define __BASE_EXTENSION_LOCK_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <mutex>
#include "base/synchronization/lock.h"

EXTENSION_BEGIN_DECLS

	//using NLock = base::Lock;
using NLock = std::recursive_mutex;
class EXTENSION_EXPORT NAutoLock
{
public:
	NAutoLock(NLock *lock) : lock_(lock) 
	{
		assert(lock);
		lock_->lock();
	}

	~NAutoLock()
	{
		if (lock_)
			lock_->unlock();
	}

private:
	NLock *lock_;
	DISALLOW_COPY_AND_ASSIGN(NAutoLock);
};

class EXTENSION_EXPORT NAutoUnlock
{
public:
	NAutoUnlock(NLock *lock) : lock_(lock)
	{
		assert(lock);
		lock_->lock();
	}

	~NAutoUnlock()
	{
		if (lock_)
			lock_->unlock();
	}

private:
	NLock *lock_;
	DISALLOW_COPY_AND_ASSIGN(NAutoUnlock);
};

EXTENSION_END_DECLS
#endif // __BASE_EXTENSION_LOCK_H__
