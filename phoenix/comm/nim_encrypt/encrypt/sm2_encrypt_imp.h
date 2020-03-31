#ifndef COMM_NIM_ENCRYPT_ENCRYPT_SM2_ENCRYPT_IMP_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_SM2_ENCRYPT_IMP_H_

#include "nim_encrypt/config/build_config.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "nim_encrypt/wrapper/nim_encrypt_interface.h"

NIMENCRYPT_BEGIN_DECLS

class SM2EncryptMethodImp : public ISM2EncryptMethod
{
	enum class SM2Error
	{
		SM2Error_BEGIN = -1,
		SUCCESS = 0,
		INVALID_NULL_VALUE_INPUT,
		INVALID_INPUT_LENGTH,
		CREATE_SM2_KEY_PAIR_FAIL,
		COMPUTE_SM3_DIGEST_FAIL,
		ALLOCATION_MEMORY_FAIL,
		COMPUTE_SM2_SIGNATURE_FAIL,
		INVALID_SM2_SIGNATURE,
		VERIFY_SM2_SIGNATURE_FAIL,
		EC_POINT_IS_AT_INFINITY,
		COMPUTE_SM2_CIPHERTEXT_FAIL,
		COMPUTE_SM2_KDF_FAIL,
		INVALID_SM2_CIPHERTEXT,
		SM2_DECRYPT_FAIL,
		SM2Error_END
	};
public:
	SM2EncryptMethodImp();
	virtual ~SM2EncryptMethodImp();
	virtual void SetPublicKey(const std::string& hex_x, const std::string& hex_y) override;
	virtual void SetPirvateKey(const std::string hex_key) override;
	virtual bool PublicKeyEncrypt(const std::string& raw_content, std::string& encrypted_content) override;
	virtual bool PrivateKeyDecrypt(const std::string& raw_content, std::string& decrypted_content) override;
private:
	SM2Error nim_sm2_encrypt(const std::string& raw_content,const std::string& pub_key,std::string& c1,std::string& c2,std::string& c3);
	SM2Error nim_sm2_decrypt(const std::string&c1, const std::string&c2, const std::string&c3,const std::string&pri_key,std::string& decrypted_content);
private:
	std::string public_key_;
	std::string private_key_;
private:
	static const int kSM2_STRING_KEY_X_LENGTH;//公钥X坐标
	static const int kSM2_STRING_KEY_Y_LENGTH;	//公钥Y坐标
	static const int kSM2_STRING_KEY_D_LENGTH;	//私钥
	static const int kSM2_KEY_PUBLIC_LENGTH;	//公钥长度
	static const int kSM2_KEY_PRIVATE_LENGTH;//私钥长度
	static const int kSM2_SM3DIGEST_BUFFER_LENGTH;//sm3摘要
};

NIMENCRYPT_END_DECLS

#endif
