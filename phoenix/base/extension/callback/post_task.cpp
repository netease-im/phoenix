#include "extension/callback/post_task.h"
#include "extension/thread/thread_manager.h"
#include "base/bind.h"

EXTENSION_BEGIN_DECLS

bool PostTask(const StdClosure &task)
{
	return NS_EXTENSION::ThreadManager::PostTask(task);
}
bool PostTask(int64_t identifier, const StdClosure &task)
{
	return NS_EXTENSION::ThreadManager::PostTask(identifier,task);
}
bool PostTask(TaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task)
{
	DCHECK(!!task_runner);
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostTask(from_here, closure);
	return true;
}

bool PostDelayedTask(const StdClosure &task, TimeDelta delay)
{
	return NS_EXTENSION::ThreadManager::PostDelayedTask(task,delay);
}
bool PostDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay)
{
	return NS_EXTENSION::ThreadManager::PostDelayedTask(identifier,task,delay);
}

bool PostDelayedTask(TaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task, TimeDelta delay)
{
	DCHECK(!!task_runner);
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostDelayedTask(from_here, closure,delay);
	return true;
}
bool PostNonNestableTask(const StdClosure &task)
{
	return NS_EXTENSION::ThreadManager::PostNonNestableTask(task);
}
bool PostNonNestableTask(int64_t identifier, const StdClosure &task)
{
	return NS_EXTENSION::ThreadManager::PostNonNestableTask(identifier,task);
}

bool PostNonNestableTask(SingleThreadTaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task)
{
	DCHECK(!!task_runner);
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostNonNestableTask(from_here, closure);
	return true;
}
bool PostNonNestableDelayedTask(const StdClosure &task, TimeDelta delay)
{
	return NS_EXTENSION::ThreadManager::PostNonNestableDelayedTask(task,delay);
}
bool PostNonNestableDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay)
{
	return NS_EXTENSION::ThreadManager::PostNonNestableDelayedTask(identifier,task,delay);
}
bool PostNonNestableDelayedTask(SingleThreadTaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task, TimeDelta delay)
{
	DCHECK(!!task_runner);
	auto closure = base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, task);
	task_runner->PostNonNestableDelayedTask(from_here, closure,delay);
	return true;
}

bool PostTaskAndReply(int64_t identifier, const StdClosure &task, const StdClosure &reply)
{
	return NS_EXTENSION::ThreadManager::PostTaskAndReply(identifier,task,reply);
}


EXTENSION_END_DECLS