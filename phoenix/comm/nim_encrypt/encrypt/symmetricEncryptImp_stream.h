#ifndef COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_STREAM_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_SYMMETRICENCRYPTIMP_STREAM_H_

#include "nim_encrypt/config/build_config.h"
#include <openssl/evp.h>
#include "nim_encrypt/encrypt/encrypt_impl.h"

NIMENCRYPT_BEGIN_DECLS

class SymmetricEncryptImp_Stream : public SymmetricEncryptBase
{	
public:
	SymmetricEncryptImp_Stream(EncryptMethod method);
	~SymmetricEncryptImp_Stream();
public:
	virtual bool Encrypt(std::string &data) override;
	virtual bool Encrypt(const std::string &sdata, std::string &ddata) override;
	virtual bool Encrypt(const void *sdata, size_t ssize, std::string &ddata) override;
	virtual bool Decrypt(std::string &data) override;
	virtual bool Decrypt(const std::string &sdata, std::string &ddata) override;
	virtual bool Decrypt(const void *sdata, size_t ssize, std::string &ddata) override;
private:
	static const EVP_CIPHER* CreateChiper(EncryptMethod method);
	size_t EncodeSize(size_t size);
	size_t DecodeSize(size_t size);
	size_t Encrypt(const char *ibuf, size_t isize, char *obuf, size_t obsize);
	size_t Decrypt(const char *ibuf, size_t isize, char *obuf, size_t obsize);
};

NIMENCRYPT_END_DECLS

#endif

