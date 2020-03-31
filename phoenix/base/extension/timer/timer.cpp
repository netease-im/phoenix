#include "extension/timer/timer.h"
#include "extension/callback/bind_extension.h"

EXTENSION_BEGIN_DECLS

void OneShotTimer::Start(TimeDelta delay, const StdClosure& callback)
{
	scoped_refptr<SingleThreadTaskRunner> task_runner = task_runner_;
	if (task_runner == nullptr && base::MessageLoop::current() != nullptr)
		task_runner = base::MessageLoop::current()->task_runner();
	if(task_runner == nullptr)
		task_runner = ThreadManager::GlobalTimerTaskRunner();	
	if (task_runner != nullptr )
	{
		if(task_runner != task_runner_)
			SetTaskRunner(task_runner);
		base::OneShotTimer::Start(FROM_HERE, delay,
			base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, callback));
	}	
}
void RepeatingTimer::Start(TimeDelta delay, const StdClosure& callback)
{
	scoped_refptr<SingleThreadTaskRunner> task_runner = task_runner_;
	if (task_runner == nullptr && base::MessageLoop::current() != nullptr)
		task_runner = base::MessageLoop::current()->task_runner();
	if (task_runner == nullptr)
		task_runner = ThreadManager::GlobalTimerTaskRunner();
	if (task_runner != nullptr)
	{
		if (task_runner != task_runner_)
			SetTaskRunner(task_runner);		
		base::RepeatingTimer::Start(FROM_HERE, delay, 
			base::Bind(base::extension::InvokeCallback<typename std::result_of<StdClosure()>::type>, callback));
	}
}

EXTENSION_END_DECLS