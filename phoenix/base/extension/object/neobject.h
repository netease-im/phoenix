#ifndef __BASE_EXTENSION_NE_OBJECT_H__
#define __BASE_EXTENSION_NE_OBJECT_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <memory>

EXTENSION_BEGIN_DECLSz
	class EXTENSION_EXPORT neobject : public std::enable_shared_from_this<neobject>
	{
	public:
		neobject()  = default;

	protected:
		virtual ~neobject()  = default;
	};

EXTENSION_END_DECLS
#endif //__BASE_EXTENSION_NE_OBJECT_H__
