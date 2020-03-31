#include "nim_encrypt/encrypt/symmetricEncryptImp_stream.h"
NIMENCRYPT_BEGIN_DECLS
SymmetricEncryptImp_Stream::SymmetricEncryptImp_Stream(EncryptMethod method) :
	SymmetricEncryptBase(method)
{

}
SymmetricEncryptImp_Stream::~SymmetricEncryptImp_Stream()
{

}
const EVP_CIPHER* SymmetricEncryptImp_Stream::CreateChiper(EncryptMethod method)
{
	switch (method)
	{
	case EncryptMethod::ENC_ARC4:
		return EVP_rc4();
	default:
		return nullptr;
	}
	return nullptr;
}
size_t SymmetricEncryptImp_Stream::EncodeSize(size_t size) 
{ 
	return size; 
}
size_t SymmetricEncryptImp_Stream::DecodeSize(size_t size) 
{ 
	return size; 
}
bool SymmetricEncryptImp_Stream::Encrypt(std::string &data)
{
	return Encrypt(data.data(), data.size(), data);
}
bool SymmetricEncryptImp_Stream::Decrypt(std::string &data)
{
	return Decrypt(data.data(), data.size(), data);
}
bool SymmetricEncryptImp_Stream::Encrypt(const void *sdata, size_t isize, std::string &ddata)
{
	size_t osize = EncodeSize(isize);
	if (osize > ddata.size())
		ddata.resize(osize);
	char  *ptr = (char *)&ddata[0];
	size_t sz = Encrypt((const char *)sdata, isize, ptr, osize);
	if (ddata.size() > osize)
		ddata.resize(osize);
	return true;
}
bool SymmetricEncryptImp_Stream::Decrypt(const void *sdata, size_t isize, std::string &ddata)
{
	size_t osize = DecodeSize(isize);
	if (osize > ddata.size())
		ddata.resize(osize);
	char  *ptr = (char *)&ddata[0];
	size_t sz = Decrypt((const char *)sdata, isize, ptr, osize);
	if (ddata.size() > osize)
		ddata.resize(osize);
	return true;
}
bool SymmetricEncryptImp_Stream::Encrypt(const std::string &sdata, std::string &ddata)
{
	return Encrypt(sdata.data(), sdata.size(), ddata);
}
bool  SymmetricEncryptImp_Stream::Decrypt(const std::string &sdata, std::string &ddata)
{
	return Decrypt(sdata.data(), sdata.size(), ddata);
}
size_t SymmetricEncryptImp_Stream::Encrypt(const char *ibuf, size_t isize, char *obuf, size_t obsize)
{
	if (isize == 0)
		return 0;
	auto chiper = CreateChiper(method_);
	ExpandKey(EVP_CIPHER_key_length(chiper), key_);
	auto ctx = EVP_CIPHER_CTX_new();
	if (!EVP_EncryptInit(ctx, chiper, (const unsigned char*)key_.c_str(), (iv_parameter_spec_.empty() ? nullptr : (const unsigned char*)iv_parameter_spec_.c_str())))
		return 0;
	int lout;
	if (!EVP_EncryptUpdate(ctx, (unsigned char *)obuf, &lout, (const unsigned char *)ibuf, (int)isize))
	{
		return 0;
	}
	size_t osize = lout;
	if (!EVP_EncryptFinal(ctx, (unsigned char *)&obuf[lout], &lout))
	{
		return 0;
	}
	osize += (size_t)lout;
	EVP_CIPHER_CTX_free(ctx);
	return osize;
}
size_t SymmetricEncryptImp_Stream::Decrypt(const char *ibuf, size_t isize, char *obuf, size_t obsize)
{
	if (isize == 0)
		return 0;
	auto chiper = CreateChiper(method_);
	ExpandKey(EVP_CIPHER_key_length(chiper), key_);
	auto ctx = EVP_CIPHER_CTX_new();
	if (!EVP_DecryptInit(ctx, chiper, (const unsigned char*)key_.c_str(), (iv_parameter_spec_.empty() ? nullptr : (const unsigned char*)iv_parameter_spec_.c_str())))
		return 0;
	int lout;
	if (!EVP_DecryptUpdate(ctx, (unsigned char *)obuf, &lout, (const unsigned char *)ibuf, (int)isize))
	{
		return 0;
	}
	size_t osize = (size_t)lout;
	if (!EVP_DecryptFinal(ctx, (unsigned char *)&obuf[lout], &lout))
	{
		return 0;
	}
	osize += (size_t)lout;
	EVP_CIPHER_CTX_free(ctx);
	return osize;
}
NIMENCRYPT_END_DECLS