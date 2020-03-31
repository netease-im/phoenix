#include "extension/thread/framework_thread.h"
#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/bind.h"

EXTENSION_BEGIN_DECLS

base::LazyInstance<base::ThreadLocalPointer<FrameworkThreadTlsData>>::Leaky
	lazy_framework_thread_tls = LAZY_INSTANCE_INITIALIZER;


int64_t FrameworkThread::CurrentManagedThreadId()
{
	FrameworkThreadTlsData *tls = GetTlsData();
	if (!tls)
		return -1;
	return tls->managed_thread_id;
}
void FrameworkThread::RegisterInitCallback(StdClosure closure)
{
	_init_callback = std::move(closure);
}
void FrameworkThread::RegisterCleanupCallback(StdClosure closure)
{
	_clean_callback = std::move(closure);
}
void FrameworkThread::Init()
{
	FrameworkThread::InitTlsData(this);
	if (_thread_register != nullptr)
		_thread_register(true);
	if (_init_callback)	{
		_init_callback();
	}
}
void FrameworkThread::CleanUp()
{
	if (_clean_callback) {
		_clean_callback();
	}
	if (_thread_register != nullptr)
		_thread_register(false);
	FrameworkThread::FreeTlsData();
}

void FrameworkThread::InitTlsData(FrameworkThread *self)
{
	FrameworkThreadTlsData *tls = FrameworkThread::GetTlsData();
	if (tls)
		return;
	tls = new FrameworkThreadTlsData;
	tls->self = self;
	tls->managed = 0;
	tls->managed_thread_id = -1;
	tls->custom_data = nullptr;
	lazy_framework_thread_tls.Pointer()->Set(tls);
}
void FrameworkThread::FreeTlsData()
{

	FrameworkThreadTlsData *tls = FrameworkThread::GetTlsData();
	if (!tls)
		return;
	lazy_framework_thread_tls.Pointer()->Set(nullptr);
	delete tls;
}
FrameworkThreadTlsData* FrameworkThread::GetTlsData()
{
	return lazy_framework_thread_tls.Pointer()->Get();
}

bool FrameworkThread::IsTlsDataSet()
{
	return !!lazy_framework_thread_tls.Pointer()->Get();
}
scoped_ptr<MessagePump> MessagePumpFactory(const MessageLoop::Type type)
{
	return MessageLoop::CreateMessagePumpForType(type);
}
void FrameworkThread::AttachCurrentThreadWithLoop(const MessageLoop::Type type)
{
	InitTlsData(this);
	thread_ = PlatformThread::CurrentHandle();
	SetThreadWasQuitProperly(false);	
	_attached_some_where = true;	
	message_loop_ = new MessageLoop(MessageLoop::CreateMessagePumpForType(type));
	ThreadMain();		
	FreeTlsData();
}
void FrameworkThread::DettachCurrentThread()
{
	if ((!_attached_some_where) || (GetThreadId() != PlatformThread::CurrentId()))
		return;
	if (stopping_ || !message_loop_)
		return;
	stopping_ = true;
	message_loop_->QuitWhenIdle();
	SetThreadWasQuitProperly(true);
	thread_ = base::PlatformThreadHandle();
}
EXTENSION_END_DECLS