#ifndef __BASE_EXTENSION_BIND_EXTENSION_H__
#define __BASE_EXTENSION_BIND_EXTENSION_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/callback/callback.h"
#include <functional>

EXTENSION_BEGIN_DECLS

template<typename R, typename... Args>
auto InvokeCallback(const std::function<R(Args...)>& f,Args && ... args)->R
{
	return f(std::forward<Args>(args)...);
}

EXTENSION_END_DECLS

#endif //__BASE_EXTENSION_BIND_EXTENSION_H__