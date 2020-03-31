#ifndef COMM_NIM_ENCRYPT_WAPPER_NIM_ENCRYPT_INTERFACE_H_
#define COMM_NIM_ENCRYPT_WAPPER_NIM_ENCRYPT_INTERFACE_H_
#include "nim_encrypt/config/build_config.h"
#include <string>
#include <memory>
NIMENCRYPT_BEGIN_DECLS

enum class EncryptMethod
{
	ENC_BEGIN = 0,
	//stream
	ENC_ARC4,
	// block
	ENC_RC2,
	ENC_CAST,
	ENC_AES128,
	ENC_AES192,
	ENC_AES256,
	ENC_DES64,
	ENC_AES128_CBC,
	ENC_AES256_CBC,
	ENC_SM4_ECB,
	//hash
	ENC_MD4,
	ENC_MD5,
	ENC_SHA1,
	ENC_SM3,
	ENC_END
};

class ISymmetricCipher
{
public:
	virtual EncryptMethod Method() const = 0;
	virtual void SetKey(const std::string &key) = 0;
	virtual std::string GetKey() const = 0;
	virtual void SetIvParameterSpec(const std::string &iv) = 0;
	virtual std::string GetIvParameterSpec() const = 0;
	virtual void EnableEncryptPadding(bool enable, int padding_mode) = 0;
	virtual void UpdatePaddingMode(int padding_mode) = 0;
};
// interface of encrypt algorithm
class ISymmetricEncryptInterface : virtual public ISymmetricCipher
{
public:
	virtual bool Encrypt(std::string &data) = 0;
	virtual bool Encrypt(const std::string &sdata, std::string &ddata) = 0;
	virtual bool Encrypt(const void *sdata, size_t ssize, std::string &ddata) = 0;
};
// interface of decrypt algorithm
class ISymmetricDecryptInterface : virtual public ISymmetricCipher
{
public:
	virtual bool Decrypt(std::string &data) = 0;
	virtual bool Decrypt(const std::string &sdata, std::string &ddata) = 0;
	virtual bool Decrypt(const void *sdata, size_t ssize, std::string &ddata) = 0;
};
class ISymmetricEncryptMethod : public ISymmetricEncryptInterface, public ISymmetricDecryptInterface
{

};
enum class RSAPaddingMode
{	
	RSAPaddingMode_Begin = 0,
	PKCS1_PADDING,
	SSLV23_PADDING,
	NO_PADDING,
	PKCS1_OAEP_PADDING,
	X931_PADDING,
	RSAPaddingMode_End
};
class IRSACipher
{
public:
	virtual void SetPublicKey(const std::string& hex_mode, const std::string& hex_exp) = 0;
	virtual void SetPirvateKey(const std::string& hex_key) = 0;
	virtual void SetPaddingMode(RSAPaddingMode mode) = 0;
	virtual bool PublicKeyEncrypt(const std::string& raw_content, std::string& encrypted_content) = 0;
	virtual bool PrivateKeyDecrypt(const std::string& raw_content, std::string& decrypted_content) = 0;
	virtual bool PrivateKeyEncrypt(const std::string& raw_content, std::string& encrypted_content) = 0;
	virtual bool PublicKeyDecrypt(const std::string& raw_content, std::string& decrypted_content) = 0;
};
class IRSAEncryptMethod : public IRSACipher
{

};
class ISM2Cipher
{
public:
	virtual void SetPublicKey(const std::string& hex_x, const std::string& hex_y) = 0;
	virtual void SetPirvateKey(const std::string hex_key) = 0;
	virtual bool PublicKeyEncrypt(const std::string& raw_content, std::string& encrypted_content) = 0;
	virtual bool PrivateKeyDecrypt(const std::string& raw_content, std::string& decrypted_content) = 0;
};
class ISM2EncryptMethod : public ISM2Cipher
{

};
NIMENCRYPT_END_DECLS
#endif
