#include "nim_encrypt/wrapper/nim_encrypt.h"
#include "nim_encrypt/encrypt/encrypt_utli.h"
#include "nim_encrypt/encrypt/rsa_encrypt_imp.h"
#include "nim_encrypt/encrypt/sm2_encrypt_imp.h"
#include "nim_encrypt/encrypt/symmetricEncryptImp_block.h"
#include "nim_encrypt/encrypt/symmetricEncryptImp_stream.h"
#include "nim_encrypt/encrypt/symmetricEncryptImp_hash.h"

NIMENCRYPT_BEGIN_DECLS

std::string NIMEncrypt::BinaryToHexString(const std::string& binary)
{
	return EncryptUtli::BinaryToHexString(binary);
}

SymmetricEncryptMethod NIMEncrypt::CreateMethod(EncryptMethod method)
{
	switch (method)
	{
	case EncryptMethod::ENC_ARC4:
		return std::make_shared<SymmetricEncryptImp_Stream>(method);
	case EncryptMethod::ENC_RC2:
	case EncryptMethod::ENC_CAST:
	case EncryptMethod::ENC_AES128:
	case EncryptMethod::ENC_AES192:
	case EncryptMethod::ENC_AES256:
	case EncryptMethod::ENC_DES64:
	case EncryptMethod::ENC_AES128_CBC:
	case EncryptMethod::ENC_AES256_CBC:
	case EncryptMethod::ENC_SM4_ECB:
		return std::make_shared<SymmetricEncryptImp_Block>(method);
	case EncryptMethod::ENC_MD4:
	case EncryptMethod::ENC_MD5:
	case EncryptMethod::ENC_SHA1:
	case EncryptMethod::ENC_SM3:
		return std::make_shared<SymmetricEncryptImp_Hash>(method);
	}
	return nullptr;
}
RSAEncryptMethod NIMEncrypt::CreateRSAMethod()
{
	return std::make_shared<RSAEncryptMethodImp>();
}
SM2EncryptMethod NIMEncrypt::CreateSM2Method()
{
	return std::make_shared<SM2EncryptMethodImp>();
}
NIMENCRYPT_END_DECLS