// Thread with framework(message loop)

#ifndef BASE_THREAD_FRAMEWORK_THREAD_H_
#define BASE_THREAD_FRAMEWORK_THREAD_H_

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/callback/callback.h"
#include "base/macros.h"
#include "base/threading/thread.h"

EXTENSION_BEGIN_DECLS

class FrameworkThread;
struct EXTENSION_EXPORT FrameworkThreadTlsData
{
	FrameworkThread *self;		// A pointer pointed to the thread itself
	int64_t managed;			// reference counter, the framework thread is managed by ThreadManager
	int64_t managed_thread_id;	// if a thread is managed, this will be the manager id of it (may not be a thread id)
	void *custom_data;			// reserved for the derived classes of FrameworkThread
};

class EXTENSION_EXPORT FrameworkThread :public base::Thread
{
	friend class ThreadMap;
	friend class ThreadManager;
public:
	explicit FrameworkThread(const std::string& name):base::Thread(name), _attached_some_where(false){}

	// Get the managed thread id of current thread
	// return -1 means current thread is not managed by ThreadManager
	static int64_t CurrentManagedThreadId();

	void RegisterInitCallback(StdClosure closure);
	void RegisterCleanupCallback(StdClosure closure);

	void AttachCurrentThreadWithLoop(const MessageLoop::Type type);
	void DettachCurrentThread();
protected:
	virtual void Init() override;
	// Called just after the message loop ends
	virtual void CleanUp() override;
	// Initilize the tls data, must be called before any tls associated calls on current thread
public:
	static FrameworkThreadTlsData* GetTlsData();
protected:
	static void InitTlsData(FrameworkThread *self);
	// Free the tls data, must be called after any tls associated calls on current thread
	static void FreeTlsData();
	// Retrieve the tls data		
	static bool IsTlsDataSet();
private:
	void SetManagerRegister(const std::function<void(bool)>& thread_register) { 
		_thread_register = thread_register;
	};
private:
	StdClosure _init_callback;
	StdClosure _clean_callback;
	std::function<void(bool)> _thread_register;
	bool _attached_some_where;
	DISALLOW_COPY_AND_ASSIGN(FrameworkThread);
};

EXTENSION_END_DECLS

#endif  // BASE_THREAD_FRAMEWORK_THREAD_H_