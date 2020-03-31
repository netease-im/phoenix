#ifndef COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_HASH_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_HASH_H_

#include "nim_encrypt/config/build_config.h"
#include <openssl/evp.h>
#include "nim_encrypt/encrypt/encrypt_impl.h"

NIMENCRYPT_BEGIN_DECLS
class SymmetricEncryptImp_Hash : public SymmetricEncryptBase
{
public:
	SymmetricEncryptImp_Hash(EncryptMethod method);
	~SymmetricEncryptImp_Hash();
public:
	virtual bool Encrypt(std::string &data) override;
	virtual bool Encrypt(const std::string &sdata, std::string &ddata) override;
	virtual bool Encrypt(const void *sdata, size_t ssize, std::string &ddata) override;
	virtual bool Decrypt(std::string &data) override;
	virtual bool Decrypt(const std::string &sdata, std::string &ddata) override;
	virtual bool Decrypt(const void *sdata, size_t ssize, std::string &ddata) override;
private:
	static const EVP_MD* CreateMD(EncryptMethod method);
};
NIMENCRYPT_END_DECLS

#endif//COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_HASH_H_
