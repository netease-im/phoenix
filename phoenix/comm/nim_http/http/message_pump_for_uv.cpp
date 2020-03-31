#if defined(OS_MACOSX)
#include <mach/thread_policy.h>
#endif
#include <iostream>

#include "libuv/uv.h"

#include "base/auto_reset.h"
#include "base/logging.h"
#include "base/threading/thread_restrictions.h"
#include "base/threading/thread_local.h"
#include "base/message_loop/message_loop.h"
#include "base/no_destructor.h"
#if defined(OS_MACOSX)
#include <mach/thread_policy.h>
#include "base/mac/mach_logging.h"
#include "base/mac/scoped_mach_port.h"
#include "base/mac/scoped_nsautorelease_pool.h"
#endif

#include "extension/thread/framework_thread.h"

#include "nim_http/http/message_pump_for_uv.h"

HTTP_BEGIN_DECLS

static void OnUVAsync(uv_async_t* handle)
{
	// Nothing to do
}

static void OnUVTimer(uv_timer_t* handle)
{
	// Nothing to do
}

MessagePumpForUV::Watcher::Watcher(int fd, bool socket)
	: fd_(fd), is_socket_(socket),
	closing_(false), pump_(nullptr), handle_(nullptr)
{

}

MessagePumpForUV::Watcher::~Watcher()
{
	// We should not call the destructor directly
	//	DCHECK(closing_);
	// Free the uv handle
	if (handle_ != nullptr) {
		if (!uv_is_closing((uv_handle_t *)handle_)) {
			uv_poll_stop(handle_);
			// Delayed close & delete
			uv_close((uv_handle_t *)handle_, OnUVClose);
		}
		//uv_poll_stop(handle_);
		delete handle_;
		handle_ = nullptr;
	}
}

bool MessagePumpForUV::Watcher::StartPoll(MessagePumpForUV *pump, Event mode)
{
	//	DCHECK(!closing_);
	if (pump_ != nullptr) {
		//		DCHECK(pump_ == pump);
		if (pump_ != pump) {
			//LOG_ERR(L"Alreay attached to another pump");
			return false;
		}
	}
	// Allocate a handle if we have not do so.
	if (handle_ == nullptr) {
		handle_ = new uv_poll_t();
		int r = is_socket_ ?
			uv_poll_init_socket(pump->loop_, handle_, fd_) :
			uv_poll_init(pump->loop_, handle_, fd_);
		if (r) {
			//LOG_ERR(L"Init socket failed");
			return false;
		}
		handle_->data = this;
	}
	// Change poll mode
	if (uv_poll_start(handle_, mode, MessagePumpForUV::OnLibuvNotification)) {
		//LOG_ERR(L"Poll start failed");
		return false;
	}
	pump_ = pump;
	return true;
}

void MessagePumpForUV::Watcher::OnUVClose(uv_handle_t *handle)
{
	MessagePumpForUV::Watcher *self =
		reinterpret_cast<MessagePumpForUV::Watcher *>(handle->data);
	self->InternalRelease();
}

void MessagePumpForUV::Watcher::CloseAndAutoRelease()
{
	if (closing_)
		return;
	closing_ = true;
	if (pump_ == nullptr) {
		InternalRelease();
		return;
	}

	// The watcher is being referenced by libuv, so we should close it.
	if (!uv_is_closing((uv_handle_t *)handle_)) {
		uv_poll_stop(handle_);
		// Delayed close & delete
		uv_close((uv_handle_t *)handle_, OnUVClose);
	}
}

void MessagePumpForUV::Watcher::InternalRelease()
{
	OnBeingDestroyed();
	delete this;
}


MessagePumpForUV::MessagePumpForUV() :keep_running_(true)
{
    loop_ =  new uv_loop_t();
	wakuper_ = new uv_async_t();
	timer_ = new uv_timer_t();
	uv_loop_init(loop_);
	uv_async_init(loop_, wakuper_, OnUVAsync);
	uv_timer_init(loop_, timer_);
}

MessagePumpForUV::~MessagePumpForUV()
{
	uv_stop(loop_);
	uv_close((uv_handle_t*)wakuper_,0);
	uv_timer_stop(timer_);
	//uv_loop_delete(loop_);
	uv_loop_close(loop_);

	delete timer_;
	delete wakuper_;
	delete loop_;
	loop_ = nullptr;
	timer_ = nullptr;
	wakuper_ = nullptr;
}

void MessagePumpForUV::Run(Delegate* delegate) {
	base::AutoReset<bool> auto_reset_keep_running(&keep_running_, true);
	for (;;) {
		bool did_work = delegate->DoWork();
		if (!keep_running_ || loop_ == nullptr)
			break;
		// Poll for new events once but don't block if there are no pending
		// events
		uv_run(loop_, UV_RUN_NOWAIT);
		if (!keep_running_)
			break;

		did_work |= delegate->DoDelayedWork(&delayed_work_time_);
		if (!keep_running_)
			break;

		if (did_work)
			continue;

		did_work = delegate->DoIdleWork();
		if (!keep_running_)
			break;

		if (did_work)
			continue;

		// UV_RUN_ONCE tells libuv to only block once,
		// but to service all pending events when it wakes up.
		if (delayed_work_time_.is_null()) {
			uv_run(loop_, UV_RUN_ONCE);
		}
		else {
			NS_EXTENSION::TimeDelta delay = delayed_work_time_ - NS_EXTENSION::TimeTicks::Now();
			if (delay > NS_EXTENSION::TimeDelta()) {
				uv_timer_start(timer_, OnUVTimer, delay.ToInternalValue(), 0);
				uv_run(loop_, UV_RUN_ONCE);
				uv_timer_stop(timer_);
			}
			else {
				// It looks like delayed_work_time_ indicates a time in the
				// past, so we need to call DoDelayedWork now.
				delayed_work_time_ = NS_EXTENSION::TimeTicks();
			}
		}
}
}

void MessagePumpForUV::Quit() {
	if (keep_running_)	{
		keep_running_ = false;
		ScheduleWork();
	}
}

void MessagePumpForUV::ScheduleWork() {
	// Since this can be called on any thread, we need to ensure that our Run
	// loop wakes up.
	uv_async_send(wakuper_);
}

void MessagePumpForUV::ScheduleDelayedWork(
	const base::TimeTicks& delayed_work_time) {
	// Since this is always called from the same thread as Run(), there is nothing
	// to do as the loop is already running. It will wait in Run() with the
	// correct timeout when it's out of immediate tasks.
	// TODO(gab): Consider removing ScheduleDelayedWork() when all pumps function
	// this way (bit.ly/merge-message-pump-do-work).
	delayed_work_time_ = delayed_work_time;
	uv_async_send(wakuper_);
}

#if defined(OS_MACOSX)
void MessagePumpForUV::SetTimerSlack(base::TimerSlack timer_slack) {
	thread_latency_qos_policy_data_t policy{};
	policy.thread_latency_qos_tier = timer_slack == base::TIMER_SLACK_MAXIMUM
		? LATENCY_QOS_TIER_3
		: LATENCY_QOS_TIER_UNSPECIFIED;
	base::mac::ScopedMachSendRight thread_port(mach_thread_self());
	kern_return_t kr =
		thread_policy_set(thread_port.get(), THREAD_LATENCY_QOS_POLICY,
			reinterpret_cast<thread_policy_t>(&policy),
			THREAD_LATENCY_QOS_POLICY_COUNT);
	MACH_DVLOG_IF(1, kr != KERN_SUCCESS, kr) << "thread_policy_set";
}
#endif

bool MessagePumpForUV::WatchFileDescriptor(Watcher *watcher, Event mode)
{
	if (watcher == nullptr)
		return false;
	if (!watcher->StartPoll(this, mode)) {
		return false;
	}
	return true;

}

void MessagePumpForUV::OnLibuvNotification(uv_poll_t *req, int status, int events)
{
	Watcher *watcher = static_cast<Watcher *>(req->data);
	MessagePumpForUV *pump = watcher->pump();

	//	DCHECK(watcher);
	//	DCHECK(pump != nullptr);
	//pump->WillProcessIOEvent();
	watcher->OnNotification(watcher->fd_, (Event)events, status == -1);
	//pump->event_processed_ = true;
	//pump->DidProcessIOEvent();
}
base::ThreadLocalPointer<base::MessageLoop>* GetTLSMessageLoop() {
	static base::NoDestructor<base::ThreadLocalPointer<base::MessageLoop>> lazy_tls_ptr;
	lazy_tls_ptr.get()->Set(NS_EXTENSION::FrameworkThread::GetTlsData()->self->message_loop());
	return lazy_tls_ptr.get();
}
MessageLoopCurrentForUV::MessageLoopCurrentForUV()
{
	current_ = GetTLSMessageLoop()->Get();
}
MessageLoopCurrentForUV::~MessageLoopCurrentForUV()
{

}
// Returns an interface for the MessageLoopForUI of the current thread.
	// Asserts that IsSet().
std::shared_ptr<MessageLoopCurrentForUV> MessageLoopCurrentForUV::Get()
{
	return std::make_shared<MessageLoopCurrentForUV>();
}

scoped_refptr<base::SingleThreadTaskRunner> MessageLoopCurrentForUV::GetTaskRunner()
{	
	return current_->current()->task_runner();
}
scoped_refptr<base::SingleThreadTaskRunner> MessageLoopCurrentForUV::GetUVMessageLoopTaskRunner()
{
	return current_->task_runner();
}
bool MessageLoopCurrentForUV::WatchFileDescriptor(MessagePumpForUV::Watcher *watcher, MessagePumpForUV::Event mode)
{
	return GetMessagePump()->WatchFileDescriptor(watcher,mode);
}

MessagePumpForUV* MessageLoopCurrentForUV::GetMessagePump() const
{
	 return static_cast<MessagePumpForUV*>(current_->GetMessagePump());
}

HTTP_END_DECLS
