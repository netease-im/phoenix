#ifndef COMM_NIM_ENCRYPT_ENCRYPT_UTLI_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_UTLI_H_

#include "nim_encrypt/config/build_config.h"
#include <string>
NIMENCRYPT_BEGIN_DECLS
class EncryptUtli
{
public:
	static std::string BinaryToHexString(const std::string& binary);
};
NIMENCRYPT_END_DECLS

COMM_NIM_ENCRYPT_ENCRYPT_UTLI_H_
#endif
