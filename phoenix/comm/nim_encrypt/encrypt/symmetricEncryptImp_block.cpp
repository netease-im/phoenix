#include "nim_encrypt/encrypt/symmetricEncryptImp_block.h"

NIMENCRYPT_BEGIN_DECLS
SymmetricEncryptImp_Block::SymmetricEncryptImp_Block(EncryptMethod method) :
	SymmetricEncryptBase(method)
{

}
SymmetricEncryptImp_Block::~SymmetricEncryptImp_Block()
{

}
const EVP_CIPHER* SymmetricEncryptImp_Block::CreateChiper(EncryptMethod method)
{
	switch (method)
	{
	case EncryptMethod::ENC_RC2:
		return EVP_rc2_ecb();
	case EncryptMethod::ENC_CAST:
		return EVP_cast5_ecb();
	case EncryptMethod::ENC_AES128:
		return EVP_aes_128_ecb();
	case EncryptMethod::ENC_AES192:
		return EVP_aes_192_ecb();
	case EncryptMethod::ENC_AES256:
		return EVP_aes_256_ecb();
	case EncryptMethod::ENC_DES64:
		return EVP_des_ecb();
	case EncryptMethod::ENC_AES128_CBC:
		return EVP_aes_128_cbc();
	case EncryptMethod::ENC_AES256_CBC:
		return EVP_aes_256_cbc();	
	case EncryptMethod::ENC_SM4_ECB:
		return EVP_sm4_ecb();
	}
	return nullptr;
}
bool SymmetricEncryptImp_Block::Encrypt(std::string &data)
{
	std::string src = data;
	return Encrypt(src.data(), src.size(), data);
}
bool SymmetricEncryptImp_Block::Decrypt(std::string &data)
{
	std::string src = data;
	return Decrypt(src.data(), src.size(), data);
}
bool SymmetricEncryptImp_Block::Encrypt(const void *sdata, size_t ssize, std::string &ddata)
{
	auto chiper = CreateChiper(method_);
	if (!ddata.empty())
		ddata.clear();
	auto ctx = EVP_CIPHER_CTX_new();
	if (!EVP_EncryptInit(ctx, chiper, (const unsigned char*)key_.c_str(), (iv_parameter_spec_.empty() ? nullptr : (const unsigned char*)iv_parameter_spec_.c_str()))) {
		EVP_CIPHER_CTX_free(ctx);
		return 0;
	}
	unsigned char out[256];
	if (enable_padding_)
		EVP_CIPHER_CTX_set_padding(ctx, padding_mode_);
	uint32_t osize = sizeof(out);
	int block_size = EVP_CIPHER_block_size(chiper);
	if (block_size > 0)
		osize = (sizeof(out) / block_size) * block_size;
	uint32_t block = ((uint32_t)ssize + osize - 1) / osize;
	uint32_t elen = 0;
	int      lout;
	for (uint32_t i = 0; i < block; i++)
	{
		int lin = ((uint32_t)ssize - elen) > osize ? osize : (uint32_t)ssize - elen;
		if (!EVP_EncryptUpdate(ctx, out, &lout, ((const unsigned char*)sdata) + elen, lin))
		{
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}		
		elen += lin;
		ddata.append((const char *)out, lout);
	}
	if (!EVP_EncryptFinal(ctx, out, &lout))
	{
		EVP_CIPHER_CTX_free(ctx);
		return 0;
	}
	ddata.append((const char *)out, lout);
	EVP_CIPHER_CTX_free(ctx);
	return true;
}
bool SymmetricEncryptImp_Block::Decrypt(const void *sdata, size_t ssize, std::string &ddata)
{
	auto chiper = CreateChiper(method_);
	if (!ddata.empty())
		ddata.clear();
	auto ctx = EVP_CIPHER_CTX_new();
	if (!EVP_DecryptInit(ctx, chiper, (const unsigned char*)key_.c_str(), (iv_parameter_spec_.empty() ? nullptr : (const unsigned char*)iv_parameter_spec_.c_str()))) {
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	if (enable_padding_)
		EVP_CIPHER_CTX_set_padding(ctx, padding_mode_);
	unsigned char out[256];
	uint32_t      osize = sizeof(out); 
	int block_size = EVP_CIPHER_block_size(chiper);
	if (block_size > 0)
		osize = (sizeof(out) / block_size - 1) * block_size;
	uint32_t block = ((uint32_t)ssize + osize - 1) / osize;
	uint32_t elen = 0;
	int      lout;
	for (uint32_t i = 0; i < block; i++)
	{
		int lin = (uint32_t)ssize - elen > osize ? osize : ((uint32_t)ssize - elen);
		if (!EVP_DecryptUpdate(ctx, out, &lout, ((const unsigned char*)sdata) + elen, lin))
		{
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
		elen += lin;
		ddata.append((const char *)out, lout);
	}
	if (!EVP_DecryptFinal(ctx, out, &lout))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	ddata.append((const char *)out, lout);
	EVP_CIPHER_CTX_free(ctx);
	return true;
}
bool SymmetricEncryptImp_Block::Encrypt(const std::string &sdata, std::string &ddata)
{
	return Encrypt(sdata.data(), sdata.size(), ddata);
}
bool SymmetricEncryptImp_Block::Decrypt(const std::string &sdata, std::string &ddata)
{
	return Decrypt(sdata.data(), sdata.size(), ddata);
}

NIMENCRYPT_END_DECLS