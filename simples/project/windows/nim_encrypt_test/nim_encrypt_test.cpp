#include <iostream>
#include "nim_encrypt/wrapper/nim_encrypt.h"
//rsa key 
const std::string g_rsa_key_mode = "B61F71009A9033DEA4E1C6494FF39876981BD9B05B21EF036C55492F9F0005512C97AD2924EE08193665B2B1C86BEF232F06DA17BCD487923DF9CEF4AE28796A08F26CF4E86FE28E2352E32A92D86DFA3638D06DA6F2E65CE640E9752BE3C8FF79F8C34D5437D709AAAE6A2EE76A42FC491271C468BCF46A38A35276735D91C9";
const std::string g_rsa_key_exp = "10001";
const std::string g_rsa_private_key = "2A163FFDFCBA00529E55D285D4A51D81A28B25165D290D5C0427FD5EB4E6C353CBF34D3FE9D9DC233F6FE708B0A148C51FB3FCA869A9CF9E9B15E49EA4B493C1EA7370C7E868EC7279424BB449964EBACEF7C10A63E2CDB97775C11683E519858F56E47947BCBD16DB985820E3A97A7FDE970CA0D76E51F3DA3766A9208C6325";
//sm2 key
std::string g_sm2_key_x_ = "f1a239fdb8b48aac71cc1f85b452fd148bd08afda770a75297fb54199d35a4f1";
std::string g_sm2_key_y_ = "8522005a55d448a527e346d4cd3b674f0456692a90650d859c6ad4b34d74c6c0";
std::string g_sm2_private_key = "0415193562d0b94025ad22315a3981af36571ebdd27fb59b6d65e7e8e2e80fdf";

const std::string  g_raw_content = "He would climb the ladder must begin at the bottom.";
int main()
{
	std::string encrypt_content("");
	std::string decrypt_content("");
	std::string key("");
	std::cout << "> raw_content:" << g_raw_content << std::endl << std::endl;
	//rsa
	{
		std::cout << "-----------nim_encrypt_test rsa begin-----------" << std::endl;		
		auto rsa = NS_NIMENCRYPT::NIMEncrypt::CreateRSAMethod();
		rsa->SetPublicKey(g_rsa_key_mode, g_rsa_key_exp);
		rsa->SetPirvateKey(g_rsa_private_key);
		std::cout << "* public key encrypt" << std::endl;
		rsa->PublicKeyEncrypt(g_raw_content, encrypt_content);
		std::cout << "> result:" << NS_NIMENCRYPT::NIMEncrypt::BinaryToHexString(encrypt_content)<< std::endl;
		std::cout << "* private key decrypt" << std::endl;
		rsa->PrivateKeyDecrypt(encrypt_content, decrypt_content);
		std::cout << "> result:" << decrypt_content << std::endl;
		encrypt_content.clear();
		decrypt_content.clear();
		std::cout << "* private key encrypt" << std::endl;
		rsa->PrivateKeyEncrypt(g_raw_content, encrypt_content);
		std::cout << "> result:" << NS_NIMENCRYPT::NIMEncrypt::BinaryToHexString(encrypt_content) << std::endl;
		std::cout << "* public key decrypt" << std::endl;
		rsa->PublicKeyDecrypt(encrypt_content, decrypt_content);
		std::cout << "> result:" << decrypt_content << std::endl ;
		encrypt_content.clear();
		decrypt_content.clear();
		std::cout << "-----------nim_encrypt_test rsa end-----------" << std::endl << std::endl;
	}
	//sm2
	{
		std::cout << "-----------nim_encrypt_test sm2 begin-----------" << std::endl;
		auto sm2 = NS_NIMENCRYPT::NIMEncrypt::CreateSM2Method();
		sm2->SetPublicKey(g_sm2_key_x_, g_sm2_key_y_);
		sm2->SetPirvateKey(g_sm2_private_key);
		std::cout << "* public key encrypt" << std::endl;
		sm2->PublicKeyEncrypt(g_raw_content, encrypt_content);
		std::cout << "> result:" << NS_NIMENCRYPT::NIMEncrypt::BinaryToHexString(encrypt_content) << std::endl;
		std::cout << "* private key decrypt" << std::endl;
		sm2->PrivateKeyDecrypt(encrypt_content, decrypt_content);
		std::cout << "> result:" << decrypt_content << std::endl;		
		encrypt_content.clear();
		decrypt_content.clear();
		std::cout << "-----------nim_encrypt_test sm2 end-----------" << std::endl << std::endl;
	}
	//hash md5
	{
		std::cout << "-----------nim_encrypt_test hash md5 begin-----------" << std::endl;
		auto md5 = NS_NIMENCRYPT::NIMEncrypt::CreateMethod(NS_NIMENCRYPT::EncryptMethod::ENC_MD5);
		std::cout << "* md5 encrypt" << std::endl;
		md5->Encrypt(g_raw_content, key);
		key = NS_NIMENCRYPT::NIMEncrypt::BinaryToHexString(key);
		std::cout << "> result:" << key << std::endl;
		std::cout << "-----------nim_encrypt_test hash md5 end-----------" << std::endl << std::endl;
	}
	//stream rc4
	{
		std::cout << "-----------nim_encrypt_test stream rc4 begin-----------" << std::endl;
		auto rc4 = NS_NIMENCRYPT::NIMEncrypt::CreateMethod(NS_NIMENCRYPT::EncryptMethod::ENC_ARC4);
		rc4->SetKey(key);
		std::cout << "* rc4 encrypt" << std::endl;
		rc4->Encrypt(g_raw_content, encrypt_content);
		std::cout << "> result:" << NS_NIMENCRYPT::NIMEncrypt::BinaryToHexString(encrypt_content) << std::endl;
		std::cout << "* rc4 decrypt" << std::endl;
		rc4->Decrypt(encrypt_content, decrypt_content);
		std::cout << "> result:" << decrypt_content << std::endl;
		encrypt_content.clear();
		decrypt_content.clear();
		std::cout << "-----------nim_encrypt_test stream rc4 end-----------" << std::endl << std::endl;
	}
	//block aes192
	{
		std::cout << "-----------nim_encrypt_test block aes192 begin-----------" << std::endl;
		auto aes192 = NS_NIMENCRYPT::NIMEncrypt::CreateMethod(NS_NIMENCRYPT::EncryptMethod::ENC_AES192);
		aes192->SetKey(key);
		std::cout << "* aes192 encrypt" << std::endl;
		aes192->Encrypt(g_raw_content, encrypt_content);
		std::cout << "> result:" << NS_NIMENCRYPT::NIMEncrypt::BinaryToHexString(encrypt_content) << std::endl;
		std::cout << "* aes192 decrypt" << std::endl;
		aes192->Decrypt(encrypt_content, decrypt_content);
		std::cout << "> result:" << decrypt_content << std::endl;
		encrypt_content.clear();
		decrypt_content.clear();
		std::cout << "-----------nim_encrypt_test block aes192 end-----------" << std::endl << std::endl;
	}
}

