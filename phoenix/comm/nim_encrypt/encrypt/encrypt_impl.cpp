#include "nim_encrypt/wrapper/nim_encrypt_interface.h"
#include "nim_encrypt/encrypt/encrypt_impl.h"
#include "nim_encrypt/encrypt/symmetricEncryptImp_hash.h"
#include "nim_encrypt/encrypt/encrypt_utli.h"

NIMENCRYPT_BEGIN_DECLS

 void SymmetricEncryptBase::ExpandKey(uint32_t ksize, std::string &key)
{
	if (key.size() < 16)
	{
		std::string k;
		k.assign(key.data(), key.size());
		SymmetricEncryptImp_Hash(EncryptMethod::ENC_MD5).Encrypt(key);
	}
	if (ksize > 16)
	{
		while (key.size() < ksize)
			key = EncryptUtli::BinaryToHexString(key);
	}
	if (key.size() > ksize)
		key.resize(ksize);
}
SymmetricEncryptBase::SymmetricEncryptBase(EncryptMethod method) :
	method_(method), key_(""), enable_padding_(false), padding_mode_(0), iv_parameter_spec_("")
{
	
}
SymmetricEncryptBase::~SymmetricEncryptBase()
{

}
EncryptMethod SymmetricEncryptBase::Method() const
{
	return method_;
}
void SymmetricEncryptBase::SetKey(const std::string &key)
{
	key_ = key;
	OnSetKey();
}
std::string SymmetricEncryptBase::GetKey() const
{
	return key_;
}
void SymmetricEncryptBase::SetIvParameterSpec(const std::string &iv)
{
	iv_parameter_spec_ = iv;
}
std::string SymmetricEncryptBase::GetIvParameterSpec() const
{
	return iv_parameter_spec_;
}
void SymmetricEncryptBase::EnableEncryptPadding(bool enable, int padding_mode)
{
	enable_padding_ = enable;
	padding_mode_ = padding_mode;
}
void SymmetricEncryptBase::UpdatePaddingMode(int padding_mode)
{
	padding_mode_ = padding_mode;
}
NIMENCRYPT_END_DECLS