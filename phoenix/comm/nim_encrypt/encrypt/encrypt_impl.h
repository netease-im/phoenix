#ifndef COMM_NIM_ENCRYPT_ENCRYPT_IMPL_H_
#define COMM_NIM_ENCRYPT_ENCRYPT_IMPL_H_

#include "nim_encrypt/config/build_config.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "nim_encrypt/wrapper/nim_encrypt_interface.h"

NIMENCRYPT_BEGIN_DECLS

class SymmetricEncryptBase : public ISymmetricEncryptMethod
{
public:
	SymmetricEncryptBase(EncryptMethod method);
	virtual ~SymmetricEncryptBase();
	virtual EncryptMethod Method() const override;
	virtual void SetKey(const std::string &key) override;
	virtual std::string GetKey() const override;
	virtual void SetIvParameterSpec(const std::string &iv) override;
	virtual std::string GetIvParameterSpec() const override;
	virtual void EnableEncryptPadding(bool enable, int padding_mode) override;
	virtual void UpdatePaddingMode(int padding_mode) override;
protected:
	static void ExpandKey(uint32_t ksize, std::string &key);
	virtual void OnSetKey() {};
protected:
	EncryptMethod method_;
	std::string key_;
	bool	 enable_padding_;
	int padding_mode_;
	std::string	 iv_parameter_spec_;
};
NIMENCRYPT_END_DECLS

#endif // COMM_NIM_ENCRYPT_ENCRYPT_IMPL_H_
