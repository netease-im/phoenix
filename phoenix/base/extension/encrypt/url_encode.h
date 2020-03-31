#ifndef __BASE_EXTENSION_URL_ENCODE_H__
#define __BASE_EXTENSION_URL_ENCODE_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <string>

EXTENSION_BEGIN_DECLS

EXTENSION_EXPORT std::string URLEncode(const std::string& input);
EXTENSION_EXPORT std::string URLDecode(const std::string& input);

EXTENSION_END_DECLS

#endif // __BASE_EXTENSION_URL_ENCODE_H__
