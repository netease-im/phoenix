#ifndef __BASE_EXTENSION_TIME_H__
#define __BASE_EXTENSION_TIME_H__
#include "extension/config/build_config.h"
#include "base/time/time.h"

EXTENSION_BEGIN_DECLS
using Time = base::Time;
using TimeDelta = base::TimeDelta;
using TimeStruct = base::Time::Exploded;
using TimeTicks = base::TimeTicks;
EXTENSION_END_DECLS

#endif // __BASE_EXTENSION_TIME_H__
