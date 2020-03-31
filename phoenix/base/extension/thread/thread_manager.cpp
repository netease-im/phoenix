#include "base/atomicops.h"
#include "base/thread_task_runner_handle.h"
#include "base/task_runner_util.h"
#include "extension/thread/thread_manager.h"

EXTENSION_BEGIN_DECLS

bool ThreadMap::AquireAccess()
{
	FrameworkThreadTlsData *tls = FrameworkThread::GetTlsData();
	if (!tls || tls->managed < 1)
		return false;
	return true;
}

bool ThreadMap::RegisterThread(int64_t self_identifier)
{
	//DCHECK(self_identifier >= 0);
	if (self_identifier < 0)
		return false;

	FrameworkThreadTlsData *tls = FrameworkThread::GetTlsData();
	DCHECK(tls);
	if (!tls)
		return false;

	base::AutoLock lock(lock_);
	auto pr = threads_.insert(
			std::make_pair(self_identifier, tls->self));
	if (!pr.second)
	{
		if (pr.first->second != tls->self)
		{
			DCHECK(false); // another thread has registered with the same id
			return false;
		}
		// yes, it's me, check logic error
		DCHECK(tls->managed > 0);
		DCHECK(tls->managed_thread_id == self_identifier);
	}
	// 'self' is registered
	tls->managed++;
	tls->managed_thread_id = self_identifier;
	return true;
}

bool ThreadMap::UnregisterThread()
{
	FrameworkThreadTlsData *tls = FrameworkThread::GetTlsData();
	DCHECK(tls); // should be called by a Framework thread
	DCHECK(tls->managed > 0); // should be managed
	if (!tls || tls->managed < 1)
		return false;

	// remove from internal thread map
	// here, since tls->managed is greater than zero,
	// we must have a reference of the glabal ThreadManager instance (see RegisterThread)
	if (--tls->managed == 0)
	{
		base::AutoLock lock(lock_);
		auto iter = threads_.find(tls->managed_thread_id);
		if (iter != threads_.end()){
			threads_.erase(iter);
		}
		else{
			DCHECK(false);	// logic error, we should not come here
		}
		tls->managed_thread_id = -1;
	}
	return true;
}

// no lock
FrameworkThread* ThreadMap::InternalQueryThread(int64_t identifier) const
{
	auto iter = threads_.find(identifier);
	if (iter == threads_.end())
		return nullptr;
	return iter->second;
}

int64_t ThreadMap::GetManagedThreadId(const FrameworkThread *thread)
{
	if (!AquireAccess()) {
		DCHECK(false);
		return -1;
	}
	base::AutoLock lock(lock_);	
	for (auto iter = threads_.begin();iter != threads_.end(); iter++) {
		if (iter->second == thread)
			return iter->first;
	}
	return -1;
}
scoped_refptr<base::SingleThreadTaskRunner> ThreadMap::task_runner(int64_t identifier) const
{
	FrameworkThread* thread = InternalQueryThread(identifier);
	if (!thread)
	{
		return nullptr;
	}

	return thread->task_runner();
}

ThreadManager::ThreadManager()
{
	_threadMap = std::make_shared<ThreadMap>();
}
ThreadManager* ThreadManager::GetInstance()
{
	return base::Singleton<ThreadManager, base::LeakySingletonTraits<ThreadManager>>::get();
}
bool ThreadManager::RegisterThread(int64_t self_identifier)
{
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	if (!thread_map)
	{
		return false;
	}
	return thread_map->RegisterThread(self_identifier);
}

bool ThreadManager::UnregisterThread()
{
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	if (!thread_map)
	{
		return false;
	}
	return thread_map->UnregisterThread();
}
FrameworkThread* ThreadManager::CreateFrameworkThread(int64_t identifier, const std::string& name)
{
	auto thread = new FrameworkThread(name);
	thread->SetManagerRegister([identifier](bool reg) {
		if (reg)
			RegisterThread(identifier);
		else
			UnregisterThread();
		});
	return thread;
}
std::shared_ptr<FrameworkThread> ThreadManager::GlobalTimerThreadRef()
{
	auto weak_thread = ThreadManager::GetInstance()->_global_timer_thread;
	auto shared_thread = weak_thread.lock();
	if (!shared_thread) {
		shared_thread = std::make_shared<FrameworkThread>("global_timer_thread");
		ThreadManager::GetInstance()->SetGlobalTimerThread(shared_thread);
		bool success = shared_thread->Start();
		DCHECK(success);
		if (!success) {
			return nullptr;
		}
		shared_thread->WaitUntilThreadStarted();
	}

	return shared_thread;
}

scoped_refptr<base::SingleThreadTaskRunner> ThreadManager::GlobalTimerTaskRunner()
{
	auto weak_thread = ThreadManager::GetInstance()->_global_timer_thread;
	if (weak_thread.expired())
	{
		return nullptr;
	}

	auto thread = weak_thread.lock();
	return thread->task_runner();
}
base::PlatformThreadId ThreadManager::GlobalTimerThreadID()
{
	auto weak_thread = ThreadManager::GetInstance()->_global_timer_thread;
	if (weak_thread.expired())
	{
		return 0;
	}

	auto thread = weak_thread.lock();
	return thread->GetThreadId();
}
int64_t ThreadManager::QueryThreadId(const FrameworkThread *thread)
{
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	if (!thread_map)
	{
		return -1;
	}
	return thread_map->GetManagedThreadId(thread);
}

FrameworkThread* ThreadManager::CurrentThread()
{
	FrameworkThreadTlsData *tls = FrameworkThread::GetTlsData();
	//DCHECK(tls); // should be called by a Framework thread
	//DCHECK(tls->managed > 0); // should be managed
	
	if (!tls || tls->managed < 1)
		return nullptr;
	return tls->self;
}

bool ThreadManager::PostTask(const StdClosure &task)
{
	//MessageLoop::current()->PostTask(task);
	DCHECK(base::ThreadTaskRunnerHandle::IsSet());
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE,closure);
	return true;
}

bool ThreadManager::PostTimerTask(const StdClosure &task, const StdClosure &cb, TimeDelta delay)
{
	auto task_runner = base::ThreadTaskRunnerHandle::Get();
	task_runner = task_runner ? task_runner : GlobalTimerTaskRunner();
	auto timer_task = [task, cb, task_runner, delay]() {

		NS_EXTENSION::WeakCallbackFlag timer_;
		StdClosure time_out_task = timer_.ToWeakCallback([cb]()
		{
			cb();
		});
		auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, time_out_task);
		task_runner->PostDelayedTask(FROM_HERE,closure, delay);
		task();
		if (timer_.HasUsed())
			timer_.Cancel();
	};
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, timer_task);
	task_runner->PostTask(FROM_HERE,closure);
	return true;
}

bool ThreadManager::PostTaskAndReply(int64_t identifier, const StdClosure &task, const StdClosure &reply)
{
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	auto task_runner = thread_map ? thread_map->task_runner(identifier) : nullptr;
	if (!task_runner)
	{
		return false;
	}
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	auto reply_closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, reply);
	task_runner->PostTaskAndReply(FROM_HERE, closure,reply_closure);
	return true;
}

bool ThreadManager::PostTask(int64_t identifier, const StdClosure &task)
{
// 	std::shared_ptr<MessageLoopProxy> message_loop =
// 		ThreadMap::GetInstance()->GetMessageLoop(identifier);
// 	if (message_loop == NULL)
// 		return false;
// 	message_loop->PostTask(task);
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	auto task_runner = thread_map ? thread_map->task_runner(identifier) : nullptr;
	if (!task_runner)
	{
		return false;
	}
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostTask(FROM_HERE, closure);
	return true;
}

bool ThreadManager::PostDelayedTask(const StdClosure &task, TimeDelta delay)
{
	//MessageLoop::current()->PostDelayedTask(task, delay);
	DCHECK(base::ThreadTaskRunnerHandle::IsSet());
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(FROM_HERE, closure,delay);
	return true;
}

bool ThreadManager::PostDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay)
{
// 	std::shared_ptr<MessageLoopProxy> message_loop =
// 		ThreadMap::GetInstance()->GetMessageLoop(identifier);
// 	if (message_loop == NULL)
// 		return false;
// 	message_loop->PostDelayedTask(task, delay);
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	auto task_runner = thread_map ? thread_map->task_runner(identifier) : nullptr;
	if (!task_runner)
	{
		return false;
	}
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostDelayedTask(FROM_HERE, closure,delay);
	return true;
}

bool ThreadManager::PostNonNestableTask(const StdClosure &task)
{
// 	MessageLoop::current()->PostNonNestableTask(task);

	DCHECK(base::ThreadTaskRunnerHandle::IsSet());
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	base::ThreadTaskRunnerHandle::Get()->PostNonNestableTask(FROM_HERE, closure);
	return true;
}

bool ThreadManager::PostNonNestableTask(int64_t identifier, const StdClosure &task)
{
// 	std::shared_ptr<MessageLoopProxy> message_loop =
// 		ThreadMap::GetInstance()->GetMessageLoop(identifier);
// 	if (message_loop == NULL)
// 		return false;
// 	message_loop->PostNonNestableTask(task);
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	auto task_runner = thread_map ? thread_map->task_runner(identifier) : nullptr;
	if (!task_runner)
	{
		return false;
	}
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostNonNestableTask(FROM_HERE, closure);
 	return true;
}

bool ThreadManager::PostNonNestableDelayedTask(const StdClosure &task, TimeDelta delay)
{
	// 	MessageLoop::current()->PostNonNestableDelayedTask(task, delay);
	DCHECK(base::ThreadTaskRunnerHandle::IsSet());
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	base::ThreadTaskRunnerHandle::Get()->PostNonNestableDelayedTask(FROM_HERE, closure,delay);
 	return true;
}

bool ThreadManager::PostNonNestableDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay)
{
// 	std::shared_ptr<MessageLoopProxy> message_loop =
// 		ThreadMap::GetInstance()->GetMessageLoop(identifier);
// 	if (message_loop == NULL)
// 		return false;
// 	message_loop->PostNonNestableDelayedTask(task, delay);
	auto thread_map = ThreadManager::GetInstance()->_threadMap;
	auto task_runner = thread_map ? thread_map->task_runner(identifier) : nullptr;
	if (!task_runner)
	{
		return false;
	}
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostNonNestableDelayedTask(FROM_HERE, closure,delay);
	return true;
}

void ThreadManager::Cleanup()
{
// 	if (GlobalTimerThread::GetInstance()->IsRunning())
// 		GlobalTimerThread::GetInstance()->Stop();
}
void ThreadManager::SetGlobalTimerThread(std::weak_ptr<FrameworkThread> thread)
{
	_global_timer_thread = thread;
}
EXTENSION_END_DECLS
