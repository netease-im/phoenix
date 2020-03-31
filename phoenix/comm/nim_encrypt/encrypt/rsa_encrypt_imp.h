#ifndef COMM_NIM_ENCRYPT_ENCRYPT_RSA_ENCRYPT_IMP_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_RSA_ENCRYPT_IMP_H_

#include "nim_encrypt/config/build_config.h"
#include <map>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "nim_encrypt/wrapper/nim_encrypt_interface.h"

NIMENCRYPT_BEGIN_DECLS

class RSAEncryptMethodImp : public IRSAEncryptMethod
{
public:
	RSAEncryptMethodImp();
	virtual ~RSAEncryptMethodImp();
	virtual void SetPublicKey(const std::string& hex_mode, const std::string& hex_exp) override;
	virtual void SetPirvateKey(const std::string& hex_key) override;
	virtual void SetPaddingMode(RSAPaddingMode mode) override;
	virtual bool PublicKeyEncrypt(const std::string& raw_content, std::string& encrypted_content) override;
	virtual bool PrivateKeyDecrypt(const std::string& raw_content, std::string& decrypted_content) override;
	virtual bool PrivateKeyEncrypt(const std::string& raw_content, std::string& encrypted_content) override;
	virtual bool PublicKeyDecrypt(const std::string& raw_content, std::string& decrypted_content) override;
private:
	static const std::map<RSAPaddingMode, int> padding_size_list_;
	static const std::map<RSAPaddingMode, int> nim_padding_mode_openssl_padding_mode_;
	std::string public_key_mode_;
	std::string public_key_exp_;
	std::string private_key_;
	RSAPaddingMode padding_mode_;
};

NIMENCRYPT_END_DECLS

#endif
