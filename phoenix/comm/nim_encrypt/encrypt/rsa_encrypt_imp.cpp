#include "nim_encrypt/encrypt/rsa_encrypt_imp.h"
NIMENCRYPT_BEGIN_DECLS
const std::map<RSAPaddingMode, int> RSAEncryptMethodImp::nim_padding_mode_openssl_padding_mode_ = {
	{RSAPaddingMode::NO_PADDING,RSA_NO_PADDING},
	{RSAPaddingMode::PKCS1_PADDING,RSA_PKCS1_PADDING},
	{RSAPaddingMode::SSLV23_PADDING,RSA_SSLV23_PADDING},
	{RSAPaddingMode::PKCS1_OAEP_PADDING,RSA_PKCS1_OAEP_PADDING},
	{RSAPaddingMode::X931_PADDING,RSA_X931_PADDING}
};
const std::map<RSAPaddingMode, int> RSAEncryptMethodImp::padding_size_list_ = {
	{RSAPaddingMode::NO_PADDING,0},
	{RSAPaddingMode::PKCS1_PADDING,RSA_PKCS1_PADDING_SIZE},
	{RSAPaddingMode::SSLV23_PADDING,0},
	{RSAPaddingMode::PKCS1_OAEP_PADDING,41},
	{RSAPaddingMode::X931_PADDING,0}
};
RSAEncryptMethodImp::RSAEncryptMethodImp() :padding_mode_(RSAPaddingMode::PKCS1_PADDING)
{	
}
RSAEncryptMethodImp::~RSAEncryptMethodImp()
{		
}
void RSAEncryptMethodImp::SetPublicKey(const std::string & hex_mode, const std::string & hex_exp)
{
	public_key_mode_ = hex_mode;
	public_key_exp_ = hex_exp;
}

void RSAEncryptMethodImp::SetPirvateKey(const std::string & hex_key)
{
	private_key_ = hex_key;
}
void RSAEncryptMethodImp::SetPaddingMode(RSAPaddingMode mode)
{

}
bool RSAEncryptMethodImp::PublicKeyEncrypt(const std::string & raw_content, std::string & encrypted_content)
{
	encrypted_content.clear();
	BIGNUM *bnn, *bne, *bnd, *bnexp;
	bnn = BN_new();
	bne = BN_new();
	bnd = BN_new();
	bnexp = BN_new();
	BN_hex2bn(&bnn, public_key_mode_.c_str());
	BN_hex2bn(&bnexp, public_key_exp_.c_str());
	BN_set_word(bne, BN_get_word(bnexp));
	RSA *pRSA = RSA_new();
	RSA_set0_key(pRSA, bnn, bne, bnd);
	size_t ssize = raw_content.size();
	unsigned char buf[256];
	int  ret = -1;
	uint32_t block_size = RSA_size(pRSA) - padding_size_list_.at(padding_mode_);
	uint32_t nBlock = ((uint32_t)ssize + block_size - 1) / block_size;
	const unsigned char * psrc = (const unsigned char *)raw_content.c_str();
	for (uint32_t i = 0; i < nBlock; i++)
	{
		uint32_t sz = block_size;
		if ((i + 1) == nBlock && (ssize%block_size != 0))
		{
			sz = ssize % block_size;
		}
		ret = RSA_public_encrypt(sz, psrc, buf, pRSA, nim_padding_mode_openssl_padding_mode_.at(padding_mode_));
		if (ret > 0)
		{
			encrypted_content.append((const char *)buf, ret);
			psrc += sz;
		}
		else
		{
			break;
		}			
	}
	RSA_set0_key(pRSA, 0, 0, 0);
	RSA_free(pRSA);
	return ret > 0;
}
int password_callback(char *buf, int bufsiz, int verify, void *cb_tmp)
{
	return 0;
}
bool RSAEncryptMethodImp::PrivateKeyDecrypt(const std::string & raw_content, std::string & decrypted_content)
{
	decrypted_content.clear();
	BIGNUM *bnn, *bne, *bnd, *bnexp;
	bnn = BN_new();
	bne = BN_new();
	bnd = BN_new();
	bnexp = BN_new();
	BN_hex2bn(&bnn, public_key_mode_.c_str());	
	BN_hex2bn(&bnexp, public_key_exp_.c_str());
	BN_set_word(bne, BN_get_word(bnexp));
	BN_hex2bn(&bnd, private_key_.c_str());
	RSA *pRSA = RSA_new();
	RSA_set0_key(pRSA, bnn, bne, bnd);
	size_t ssize = raw_content.size();
	unsigned char buf[256];
	int  ret = -1;
	uint32_t block_size = RSA_size(pRSA);
	uint32_t nBlock = ((uint32_t)ssize + block_size - 1) / block_size;
	const unsigned char * psrc = (const unsigned char *)raw_content.c_str();
	for (uint32_t i = 0; i < nBlock; i++)
	{
		uint32_t sz = block_size;
		if ((i + 1) == nBlock && (ssize%block_size != 0))
		{
			sz = ssize % block_size;
		}
		ret = RSA_private_decrypt(sz, psrc, buf, pRSA, nim_padding_mode_openssl_padding_mode_.at(padding_mode_));
		if (ret > 0)
		{
			decrypted_content.append((const char *)buf, ret);
			psrc += sz;
		}
		else
		{
			break;
		}
	}
	RSA_set0_key(pRSA, 0, 0, 0);
	RSA_free(pRSA);
	return ret > 0;
}
bool RSAEncryptMethodImp::PrivateKeyEncrypt(const std::string& raw_content, std::string& encrypted_content)
{
	encrypted_content.clear();
	BIGNUM *bnn, *bne, *bnd, *bnexp;
	bnn = BN_new();
	bne = BN_new();
	bnd = BN_new();
	bnexp = BN_new();
	BN_hex2bn(&bnn, public_key_mode_.c_str());
	BN_hex2bn(&bnexp, public_key_exp_.c_str());
	BN_set_word(bne, BN_get_word(bnexp));
	BN_hex2bn(&bnd, private_key_.c_str());
	RSA *pRSA = RSA_new();
	RSA_set0_key(pRSA, bnn, bne, bnd);
	size_t ssize = raw_content.size();
	unsigned char buf[256];
	int  ret = -1;
	uint32_t block_size = RSA_size(pRSA) - padding_size_list_.at(padding_mode_);
	uint32_t nBlock = ((uint32_t)ssize + block_size - 1) / block_size;
	const unsigned char * psrc = (const unsigned char *)raw_content.c_str();
	for (uint32_t i = 0; i < nBlock; i++)
	{
		uint32_t sz = block_size;
		if ((i + 1) == nBlock && (ssize%block_size != 0))
		{
			sz = ssize % block_size;
		}
		ret = RSA_private_encrypt(sz, psrc, buf, pRSA, nim_padding_mode_openssl_padding_mode_.at(padding_mode_));
		if (ret > 0)
		{
			encrypted_content.append((const char *)buf, ret);
			psrc += sz;
		}
		else
		{
			break;
		}
	}
	RSA_set0_key(pRSA, 0, 0, 0);
	RSA_free(pRSA);
	return ret > 0;
}
bool RSAEncryptMethodImp::PublicKeyDecrypt(const std::string& raw_content, std::string& decrypted_content)
{
	decrypted_content.clear();
	BIGNUM *bnn, *bne, *bnd, *bnexp;
	bnn = BN_new();
	bne = BN_new();
	bnd = BN_new();
	bnexp = BN_new();
	BN_hex2bn(&bnn, public_key_mode_.c_str());
	BN_hex2bn(&bnexp, public_key_exp_.c_str());
	BN_set_word(bne, BN_get_word(bnexp));
	RSA *pRSA = RSA_new();
	RSA_set0_key(pRSA, bnn, bne, bnd);
	size_t ssize = raw_content.size();
	unsigned char buf[256];
	int  ret = -1;
	uint32_t block_size = RSA_size(pRSA);
	uint32_t nBlock = ((uint32_t)ssize + block_size - 1) / block_size;
	const unsigned char * psrc = (const unsigned char *)raw_content.c_str();
	for (uint32_t i = 0; i < nBlock; i++)
	{
		uint32_t sz = block_size;
		if ((i + 1) == nBlock && (ssize%block_size != 0))
		{
			sz = ssize % block_size;
		}
		ret = RSA_public_decrypt(sz, psrc, buf, pRSA, nim_padding_mode_openssl_padding_mode_.at(padding_mode_));
		if (ret > 0)
		{
			decrypted_content.append((const char *)buf, ret);
			psrc += sz;
		}
		else
		{
			break;
		}
	}
	RSA_set0_key(pRSA, 0, 0, 0);
	RSA_free(pRSA);
	return ret > 0;
}
NIMENCRYPT_END_DECLS
