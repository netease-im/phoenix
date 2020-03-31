#include "nim_encrypt/encrypt/symmetricEncryptImp_hash.h"

NIMENCRYPT_BEGIN_DECLS

SymmetricEncryptImp_Hash::SymmetricEncryptImp_Hash(EncryptMethod method) :
	SymmetricEncryptBase(method)
{

}
SymmetricEncryptImp_Hash::~SymmetricEncryptImp_Hash()
{

}
const EVP_MD* SymmetricEncryptImp_Hash::CreateMD(EncryptMethod method)
{
	switch (method)
	{
	case EncryptMethod::ENC_MD5:
		return EVP_md5();
	case EncryptMethod::ENC_MD4:
		return EVP_md4();
	case EncryptMethod::ENC_SHA1:
		return EVP_sha1();
	case EncryptMethod::ENC_SM3:
		return EVP_sm3();
	default:
		return nullptr;
	}
	return nullptr;
}
bool SymmetricEncryptImp_Hash::Encrypt(std::string &data)
{
	std::string src = data;
	return Encrypt(src.data(), src.size(), data);
}
bool SymmetricEncryptImp_Hash::Decrypt(std::string &data)
{
	std::string src = data;
	return Decrypt(src.data(), src.size(), data);
}
bool SymmetricEncryptImp_Hash::Encrypt(const std::string &sdata, std::string &ddata)
{
	return Encrypt(sdata.data(), sdata.size(), ddata);
}
bool SymmetricEncryptImp_Hash::Decrypt(const std::string &sdata, std::string &ddata)
{
	return Decrypt(sdata.data(), sdata.size(), ddata);
}
bool SymmetricEncryptImp_Hash::Encrypt(const void *sdata, size_t ssize, std::string &ddata)
{
	auto md = CreateMD(method_);
	if (!ddata.empty())
		ddata.clear();

	EVP_MD_CTX* mdctx = EVP_MD_CTX_create();

	EVP_MD_CTX_init(mdctx);
	if (!EVP_DigestInit_ex(mdctx, md, NULL))
	{
		EVP_MD_CTX_free(mdctx);
		return false;
	}
	if (!EVP_DigestUpdate(mdctx, (const unsigned char *)sdata, ssize))
	{
		EVP_MD_CTX_free(mdctx);
		return false;
	}
	unsigned char  obuf[1024];
	unsigned char *pbuf = obuf;
	unsigned int   olen;

	olen = EVP_MD_size(md);
	if (olen > sizeof(obuf))
		pbuf = new unsigned char[olen];

	if (!EVP_DigestFinal_ex(mdctx, pbuf, &olen))
	{
		if (pbuf != obuf)
			delete[] pbuf;
		EVP_MD_CTX_free(mdctx);
		return false;
	}
	ddata.append((const char *)pbuf, olen);
	if (pbuf != obuf)
		delete[] pbuf; 
	EVP_MD_CTX_free(mdctx);
	return true;
}
bool SymmetricEncryptImp_Hash::Decrypt(const void *sdata, size_t ssize, std::string &ddata)
{
	return false;
}
NIMENCRYPT_END_DECLS