#ifndef COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_BLOCK_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_BLOCK_H_

#include "nim_encrypt/config/build_config.h"
#include <openssl/evp.h>
#include "nim_encrypt/encrypt/encrypt_impl.h"

NIMENCRYPT_BEGIN_DECLS
class SymmetricEncryptImp_Block : public SymmetricEncryptBase
{
public:
	SymmetricEncryptImp_Block(EncryptMethod method);
	~SymmetricEncryptImp_Block();
public:
	virtual bool Encrypt(std::string &data) override;
	virtual bool Encrypt(const std::string &sdata, std::string &ddata) override;
	virtual bool Encrypt(const void *sdata, size_t ssize, std::string &ddata) override;
	virtual bool Decrypt(std::string &data) override;
	virtual bool Decrypt(const std::string &sdata, std::string &ddata) override;
	virtual bool Decrypt(const void *sdata, size_t ssize, std::string &ddata) override;
private:
	static const EVP_CIPHER* CreateChiper(EncryptMethod method);
};
NIMENCRYPT_END_DECLS

#endif//COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_BLOCK_H_
