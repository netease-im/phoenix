#ifndef  COMM_NIM_ENCRYPT_WAPPER_NIM_ENCRYPT_H_
#define  COMM_NIM_ENCRYPT_WAPPER_NIM_ENCRYPT_H_

#include "nim_encrypt/config/build_config.h"
#include "nim_encrypt/wrapper/nim_encrypt_interface.h"

NIMENCRYPT_BEGIN_DECLS

using SymmetricEncryptMethod = std::shared_ptr<ISymmetricEncryptMethod> ;

using RSAEncryptMethod = std::shared_ptr<IRSAEncryptMethod>;

using SM2EncryptMethod = std::shared_ptr<ISM2EncryptMethod>;

class NIMEncrypt
{
public:
	static SymmetricEncryptMethod CreateMethod(EncryptMethod method);
	static RSAEncryptMethod CreateRSAMethod();
	static SM2EncryptMethod CreateSM2Method();
	static std::string BinaryToHexString(const std::string& binary);
};
NIMENCRYPT_END_DECLS

#endif  // COMM_NIM_ENCRYPT_WAPPER_NIM_ENCRYPT_H_

