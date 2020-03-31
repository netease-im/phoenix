#ifndef BASE_EXTENSION_WRAPPERS_H_
#define BASE_EXTENSION_WRAPPERS_H_
#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "base/at_exit.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"

EXTENSION_BEGIN_DECLS

template<typename T, class D = std::default_delete<T>>
using scoped_ptr = NS_BASE::scoped_ptr<T,D>;
template <class T, class D>
using scoped_ptr_array = NS_BASE::scoped_ptr<T[], D>;

EXTENSION_END_DECLS

#endif
