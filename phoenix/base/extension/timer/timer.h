#ifndef BASE_TIME_TIMER_H_
#define BASE_TIME_TIMER_H_

#include "extension/extension_export.h"
#include "extension/config/build_config.h"

#include <assert.h>

#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/threading/sequenced_task_runner_handle.h"
#include "base/bind.h"

#include "extension/thread/thread_manager.h"
#include "extension/callback/callback.h"

EXTENSION_BEGIN_DECLS
// A simple, one-shot timer.  See usage notes at the top of the file.
class EXTENSION_EXPORT OneShotTimer : public base::OneShotTimer
{
public:
	void Start(TimeDelta delay, const StdClosure& callback);
};

//-----------------------------------------------------------------------------
// A simple, repeating timer.  See usage notes at the top of the file.
class EXTENSION_EXPORT RepeatingTimer : public base::RepeatingTimer
{
public:
	void Start(TimeDelta delay, const StdClosure& callback);
};
EXTENSION_END_DECLS
#endif  // BASE_TIME_TIMER_H_