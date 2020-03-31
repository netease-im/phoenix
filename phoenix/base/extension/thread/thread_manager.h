// a thread manager for iter-thread communicatios, etc.

#ifndef __BASE_EXTENSION_THREAD_MANAGER_H__
#define __BASE_EXTENSION_THREAD_MANAGER_H__

#include "extension/config/build_config.h"

#include <map>
#include <memory>
#include "base/synchronization/lock.h"
#include "base/memory/ref_counted.h"
#include "base/single_thread_task_runner.h"
#include "base/memory/singleton.h"

#include "extension/extension_export.h"
#include "extension/callback/bind_extension.h"
#include "extension/thread/framework_thread.h"
#include "extension/time/time.h"
#include "extension/memory/singleton.h"

EXTENSION_BEGIN_DECLS

class ThreadMap
{
	friend class ThreadManager;
public:
	ThreadMap() = default;
	bool AquireAccess();
	bool RegisterThread(int64_t self_identifier);
	bool UnregisterThread();
	int64_t GetManagedThreadId(const FrameworkThread *thread);
	scoped_refptr<base::SingleThreadTaskRunner> task_runner(int64_t identifier) const;

private:
	FrameworkThread* InternalQueryThread(int64_t identifier) const;
	base::Lock lock_;
	std::map<int, FrameworkThread*> threads_;
};


// 使用ThreadManager可以极大地方便线程间通信
// 注意：只有受ThreadManager托管的线程（通过Register托管）才允许调用除Register和Post族外的成员函数
class EXTENSION_EXPORT ThreadManager:public NS_EXTENSION::Singleton<ThreadManager>
{
public:
// 	static MessageLoop* GetTimerMessageLoop()
// 	{
// 		if (!GlobalTimerThread::GetInstance()->IsRunning())
// 			GlobalTimerThread::GetInstance()->Start();
// 		return GlobalTimerThread::GetInstance()->message_loop();
// 	}
	// 托管当前FrameworkThread线程
	// identifier >= 0
	// 必须在self的线程过程内被调用

	ThreadManager();
	static FrameworkThread* CreateFrameworkThread(int64_t identifier, const std::string& name);
	//全局初始化的时候 需要保存这个引用，并在cleanup的时候unref
	static std::shared_ptr<FrameworkThread> GlobalTimerThreadRef();
	static scoped_refptr<base::SingleThreadTaskRunner> GlobalTimerTaskRunner();
	static base::PlatformThreadId GlobalTimerThreadID();

	static ThreadManager* GetInstance();
	static bool RegisterThread(int64_t self_identifier);
	// 取消当前线程托管
	// 线程运行结束之前必须调用UnregisterThread取消托管
	static bool UnregisterThread();


	// 查找
	static FrameworkThread* CurrentThread();
	//template<typename T> static T* CurrentThreadT();
	static int64_t QueryThreadId(const FrameworkThread *thread);

	static bool PostTask(const StdClosure &task);
	static bool PostTask(int64_t identifier, const StdClosure &task);

	static bool PostDelayedTask(const StdClosure &task, TimeDelta delay);
	static bool PostDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay);

	static bool PostNonNestableTask(const StdClosure &task);
	static bool PostNonNestableTask(int64_t identifier, const StdClosure &task);

	static bool PostNonNestableDelayedTask(const StdClosure &task, TimeDelta delay);
	static bool PostNonNestableDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay);
	static bool PostTimerTask(const StdClosure &task, const StdClosure &cb, TimeDelta delay);
	static bool PostTaskAndReply(int64_t identifier, const StdClosure &task, const StdClosure &reply);
// 	template<typename T1, typename T2>
// 	static bool Await(int64_t identifier, const std::function<T1> &task, const std::function<T2> &reply)
// 	{
// 		std::shared_ptr<MessageLoopProxy> message_loop =
// 			ThreadMap::GetInstance()->GetMessageLoop(identifier);
// 		if (message_loop == NULL)
// 			return false;
// 		message_loop->PostTaskAndReply(task, reply);
// 		return true;
// 	}
	static void Cleanup();
private:
// 	static void RunRepeatedly(const WeakCallback<StdClosure>& task, const TimeDelta& delay, int times);
// 	static void RunRepeatedly2(int thread_id, const WeakCallback<StdClosure>& task, const TimeDelta& delay, int times);
	void SetGlobalTimerThread(std::weak_ptr<FrameworkThread> thread);
	std::shared_ptr<ThreadMap> _threadMap;
	std::weak_ptr<FrameworkThread> _global_timer_thread;
	DISALLOW_COPY_AND_ASSIGN(ThreadManager);
};

// template<typename T>
// T* ThreadManager::CurrentThreadT()
// {
// 	return static_cast<T *>(CurrentThread());
// }

EXTENSION_END_DECLS

#endif // __BASE_EXTENSION_THREAD_MANAGER_H__
