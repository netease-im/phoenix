#ifndef __BASE_EXTENSION_POST_TASK_H__
#define __BASE_EXTENSION_POST_TASK_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/callback/callback.h"
#include "google_base/base/single_thread_task_runner.h"

EXTENSION_BEGIN_DECLS

EXTENSION_EXPORT bool PostTask(const StdClosure &task);
EXTENSION_EXPORT bool PostTask(int64_t identifier, const StdClosure &task);
EXTENSION_EXPORT bool PostTask(TaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task);

EXTENSION_EXPORT bool PostDelayedTask(const StdClosure &task, TimeDelta delay);
EXTENSION_EXPORT bool PostDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay);
EXTENSION_EXPORT bool PostDelayedTask(TaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task, TimeDelta delay);

EXTENSION_EXPORT bool PostNonNestableTask(const StdClosure &task);
EXTENSION_EXPORT bool PostNonNestableTask(int64_t identifier, const StdClosure &task);
EXTENSION_EXPORT bool PostNonNestableTask(SingleThreadTaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task);

EXTENSION_EXPORT bool PostNonNestableDelayedTask(const StdClosure &task, TimeDelta delay);
EXTENSION_EXPORT bool PostNonNestableDelayedTask(int64_t identifier, const StdClosure &task, TimeDelta delay);
EXTENSION_EXPORT bool PostNonNestableDelayedTask(SingleThreadTaskRunner* task_runner, const tracked_objects::Location& from_here, const StdClosure &task, TimeDelta delay);

EXTENSION_EXPORT bool PostTaskAndReply(int64_t identifier, const StdClosure &task, const StdClosure &reply);


EXTENSION_END_DECLS

#endif // __BASE_EXTENSION_POST_TASK_H__
