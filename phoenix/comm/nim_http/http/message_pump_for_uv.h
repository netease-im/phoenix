#ifndef __BASE_HTTP_MESSAGE_PUMP_FOR_UV_H__
#define __BASE_HTTP_MESSAGE_PUMP_FOR_UV_H__

#include "nim_http/http_export.h"
#include "nim_http/config/build_config.h"

#include "base/macros.h"
#include "base/message_loop/message_pump.h"
#include "base/message_loop/message_loop_current.h"

#include "extension/time/time.h"


struct uv_poll_s;
struct uv_handle_s;
struct uv_loop_s;
struct uv_async_s;
struct uv_timer_s;
typedef struct uv_poll_s uv_poll_t;
typedef struct uv_loop_s uv_loop_t;
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_async_s uv_async_t;
typedef struct uv_timer_s uv_timer_t;


HTTP_BEGIN_DECLS

class HTTP_EXPORT MessagePumpForUV : public base::MessagePump
{
public:
	enum Event
	{
		NONE = 0,
		READ = 1 << 0,
		WRITE = 1 << 1,
		READ_WRITE = READ | WRITE
	};

	class Watcher
	{
	public:
		// |fd| can be a socket or file,
		// |socket| is used to specified whether |fd| is a socket
		Watcher(int fd, bool socket = true);

		// |CloseAndAutoRelease| must be called when you don't wanner use a
		//       instance any longer
		// NOTE: When |CloseAndAutoRelease| is called:
		//       * If opened, the instance will be deleted later automatically
		//       * otherwise, the instance will be deleted immediately
		void CloseAndAutoRelease();

		// Called when data is ready for non-blocking read or write
		virtual void OnNotification(int fd, Event mode, bool error) = 0;

		// Called when the instance is being destroyed
		virtual void OnBeingDestroyed() {}

	protected:
		// The destructor should ONLY be called in OnUVClose
		virtual ~Watcher();

	private:
		friend class MessagePumpForUV;

		// Called by libuv
		static void OnUVClose(uv_handle_t *handle);
		// Called by LibuvMessagePump
		MessagePumpForUV* pump() const { return pump_; }
		// Start polling with specified mode according to |event|
		bool StartPoll(MessagePumpForUV *pump, Event event);
		// Release the current instance
		void InternalRelease();

		// A socket or a file depends on |is_socket_|
		int fd_;
		// The watcher is in closing state?
		bool closing_;
		// |true| if |fd_| is a socket, otherwise |fd_| is a file
		bool is_socket_;
		// UV poll handle
		uv_poll_t *handle_;
		// The message pump that listening/dispatching the watcher's events
		MessagePumpForUV *pump_;
	};
	MessagePumpForUV();
	~MessagePumpForUV() override;

	// MessagePump methods:
	void Run(Delegate* delegate) override;
	void Quit() override;
	void ScheduleWork() override;
	void ScheduleDelayedWork(const base::TimeTicks& delayed_work_time) override;
#if defined(OS_MACOSX)
	void SetTimerSlack(base::TimerSlack timer_slack) override;
#endif

	// The following method(s) must be called on the thread
		// that the pump is running on.
		//
		// |WatchFileDescriptor| start watching of a watcher.
		// This method can be called more than once, if the watcher is already
		// running, the method won't do anything.
		//
		// The pump will NOT own the |watcher| after this method called,
		// thus to stop watching, you should call the watcher's |Close| method.
	bool WatchFileDescriptor(Watcher *watcher, Event mode);

private:
	static void OnLibuvNotification(uv_poll_t *req, int status, int events);

private:
	// This flag is set to false when Run should return.
	bool keep_running_;
	base::TimeTicks delayed_work_time_;
	// Internal uv loop
	uv_loop_t *loop_;
	// Use uv_async_send to wakup the event loop
	uv_async_t *wakuper_;
	// Used by uv_timer_start
	uv_timer_t *timer_;

	DISALLOW_COPY_AND_ASSIGN(MessagePumpForUV);
};


class HTTP_EXPORT MessageLoopCurrentForUV
{
public:
	// Returns an interface for the MessageLoopForUI of the current thread.
	// Asserts that IsSet().
	static std::shared_ptr<MessageLoopCurrentForUV> Get();

	MessageLoopCurrentForUV* operator->() { return this; }

	scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner();
	scoped_refptr<base::SingleThreadTaskRunner> GetUVMessageLoopTaskRunner();

	bool WatchFileDescriptor(MessagePumpForUV::Watcher *watcher, MessagePumpForUV::Event mode);

	explicit MessageLoopCurrentForUV();
	~MessageLoopCurrentForUV();
	MessagePumpForUV* GetMessagePump() const;
private:
	base::MessageLoop* current_;
};

HTTP_END_DECLS
#endif // __BASE_HTTP_MESSAGE_PUMP_FOR_UV_H__
