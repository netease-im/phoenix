#include "nim_encrypt/encrypt/sm2_encrypt_imp.h"
NIMENCRYPT_BEGIN_DECLS
const int SM2EncryptMethodImp::kSM2_STRING_KEY_X_LENGTH = 64;//公钥X坐标
const int SM2EncryptMethodImp::kSM2_STRING_KEY_Y_LENGTH = 64;//公钥Y坐标
const int SM2EncryptMethodImp::kSM2_STRING_KEY_D_LENGTH = 64;//私钥
const int SM2EncryptMethodImp::kSM2_KEY_PUBLIC_LENGTH = 65;//公钥长度
const int SM2EncryptMethodImp::kSM2_KEY_PRIVATE_LENGTH = 32;//私钥长度
const int SM2EncryptMethodImp::kSM2_SM3DIGEST_BUFFER_LENGTH = 32;//sm3摘要
SM2EncryptMethodImp::SM2EncryptMethodImp()
{

}
SM2EncryptMethodImp::~SM2EncryptMethodImp()
{

}
void SM2EncryptMethodImp::SetPublicKey(const std::string& hex_x, const std::string& hex_y)
{
	std::string public_key_x_ = hex_x;
	std::string public_key_y_ = hex_y;
	//补齐公钥
	std::string public_hex_key = "04";
	if (public_key_x_.length() < kSM2_STRING_KEY_X_LENGTH)
		public_key_x_.append(kSM2_STRING_KEY_X_LENGTH - public_key_x_.length(), '0');
	public_hex_key.append(public_key_x_.c_str(), kSM2_STRING_KEY_X_LENGTH);
	if (public_key_y_.length() < kSM2_STRING_KEY_Y_LENGTH)
		public_key_y_.append(kSM2_STRING_KEY_Y_LENGTH - public_key_y_.length(), '0');
	public_hex_key.append(public_key_y_.c_str(), kSM2_STRING_KEY_Y_LENGTH);
	//复制公钥
	public_key_.resize(kSM2_KEY_PUBLIC_LENGTH);
	for (int index = 0; index < kSM2_KEY_PUBLIC_LENGTH; index++)
		sscanf_s(public_hex_key.substr(2 * index, 2).c_str(), "%x", (unsigned int*)&public_key_[index]);
}
void SM2EncryptMethodImp::SetPirvateKey(const std::string hex_key)
{
	private_key_ = "";
	private_key_.resize(kSM2_KEY_PRIVATE_LENGTH);
	for (int index = 0; index < kSM2_KEY_PRIVATE_LENGTH; index++)
		sscanf_s(hex_key.substr(2 * index, 2).c_str(), "%x", (unsigned int*)&private_key_[index]);
}
bool SM2EncryptMethodImp::PublicKeyEncrypt(const std::string& raw_content, std::string& encrypted_content)
{	
	std::string c1(""), c2(""), c3("");
	c2.resize(raw_content.length());
	c3.resize(kSM2_SM3DIGEST_BUFFER_LENGTH);
	if(nim_sm2_encrypt(raw_content, public_key_, c1, c2, c3) == SM2Error::SUCCESS)
		encrypted_content.append(c1).append(c2).append(c3);
	return encrypted_content.size() > 0;
}
bool SM2EncryptMethodImp::PrivateKeyDecrypt(const std::string& raw_content, std::string& decrypted_content)
{
	std::string text_buf("");
	std::string private_key_arr("");
	decrypted_content.clear();
	decrypted_content.resize(raw_content.size());
	std::string c1(""), c2(""), c3("");
	c1.append(raw_content.substr(0, kSM2_KEY_PUBLIC_LENGTH));
	c3.append(raw_content.substr(raw_content.size() - kSM2_SM3DIGEST_BUFFER_LENGTH, kSM2_SM3DIGEST_BUFFER_LENGTH).data(), kSM2_SM3DIGEST_BUFFER_LENGTH);
	c2.append(raw_content.substr((kSM2_KEY_PUBLIC_LENGTH ), (raw_content.length() - kSM2_KEY_PUBLIC_LENGTH - kSM2_SM3DIGEST_BUFFER_LENGTH)));
	decrypted_content.resize(c2.size());
	return (nim_sm2_decrypt(c1, c2, c3, private_key_, decrypted_content) == SM2Error::SUCCESS);
}
SM2EncryptMethodImp::SM2Error SM2EncryptMethodImp::nim_sm2_encrypt(const std::string& raw_content,const std::string& pub_key,
	std::string& c1,std::string& c2,	std::string& c3)
{
	SM2Error error_code = SM2Error::SUCCESS;
	unsigned char pub_key_x[32], pub_key_y[32], c1_x[32], c1_y[32], x2[32], y2[32];
	unsigned char c1_point[65], x2_y2[64];
	unsigned char *t = NULL;
	BN_CTX *ctx = NULL;
	BIGNUM *bn_k = NULL, *bn_c1_x = NULL, *bn_c1_y = NULL;
	BIGNUM *bn_pub_key_x = NULL, *bn_pub_key_y = NULL;
	BIGNUM *bn_x2 = NULL, *bn_y2 = NULL;
	const BIGNUM *bn_order, *bn_cofactor;
	EC_GROUP *group = NULL;
	const EC_POINT *generator;
	EC_POINT *pub_key_pt = NULL, *c1_pt = NULL, *s_pt = NULL, *ec_pt = NULL;
	const EVP_MD *md;
	EVP_MD_CTX *md_ctx = NULL;
	int i, flag;

	memcpy(pub_key_x, (pub_key.data() + 1), sizeof(pub_key_x));
	memcpy(pub_key_y, (pub_key.data() + 1 + sizeof(pub_key_x)), sizeof(pub_key_y));

	error_code = SM2Error::ALLOCATION_MEMORY_FAIL;
	if (!(t = (unsigned char *)malloc(raw_content.size())))
	{
		goto clean_up;
	}
	if (!(ctx = BN_CTX_new()))
	{
		goto clean_up;
	}
	BN_CTX_start(ctx);
	bn_k = BN_CTX_get(ctx);
	bn_c1_x = BN_CTX_get(ctx);
	bn_c1_y = BN_CTX_get(ctx);
	bn_pub_key_x = BN_CTX_get(ctx);
	bn_pub_key_y = BN_CTX_get(ctx);
	bn_x2 = BN_CTX_get(ctx);
	bn_y2 = BN_CTX_get(ctx);
	if (!(bn_y2))
	{
		goto clean_up;
	}
	if (!(group = EC_GROUP_new_by_curve_name(NID_sm2)))
	{
		goto clean_up;
	}

	if (!(pub_key_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}
	if (!(c1_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}
	if (!(s_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}
	if (!(ec_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}

	if (!(md_ctx = EVP_MD_CTX_new()))
	{
		goto clean_up;
	}

	error_code = SM2Error::COMPUTE_SM2_CIPHERTEXT_FAIL;
	if (!(BN_bin2bn(pub_key_x, sizeof(pub_key_x), bn_pub_key_x)))
	{
		goto clean_up;
	}
	if (!(BN_bin2bn(pub_key_y, sizeof(pub_key_y), bn_pub_key_y)))
	{
		goto clean_up;
	}

	if (!(bn_order = EC_GROUP_get0_order(group)))
	{
		goto clean_up;
	}
	if (!(bn_cofactor = EC_GROUP_get0_cofactor(group)))
	{
		goto clean_up;
	}
	if (!(generator = EC_GROUP_get0_generator(group)))
	{
		goto clean_up;
	}

	if (!(EC_POINT_set_affine_coordinates_GFp(group,
		pub_key_pt,
		bn_pub_key_x,
		bn_pub_key_y,
		ctx)))
	{
		goto clean_up;
	}

	if (!(EC_POINT_mul(group, s_pt, NULL, pub_key_pt, bn_cofactor, ctx)))
	{
		goto clean_up;
	}
	if (EC_POINT_is_at_infinity(group, s_pt))
	{
		error_code = SM2Error::EC_POINT_IS_AT_INFINITY;
		goto clean_up;
	}
	md = EVP_sm3();

	do
	{
		if (!(BN_rand_range(bn_k, bn_order)))
		{
			goto clean_up;
		}
		if (BN_is_zero(bn_k))
		{
			continue;
		}
		if (!(EC_POINT_mul(group, c1_pt, bn_k, NULL, NULL, ctx)))
		{
			goto clean_up;
		}
		if (!(EC_POINT_mul(group, ec_pt, NULL, pub_key_pt, bn_k, ctx)))
		{
			goto clean_up;
		}
		if (!(EC_POINT_get_affine_coordinates_GFp(group,
			ec_pt,
			bn_x2,
			bn_y2,
			ctx)))
		{
			goto clean_up;
		}
		if (BN_bn2binpad(bn_x2,
			x2,
			sizeof(x2)) != sizeof(x2))
		{
			goto clean_up;
		}
		if (BN_bn2binpad(bn_y2,
			y2,
			sizeof(y2)) != sizeof(y2))
		{
			goto clean_up;
		}
		memcpy(x2_y2, x2, sizeof(x2));
		memcpy((x2_y2 + sizeof(x2)), y2, sizeof(y2));

		if (!(ECDH_KDF_X9_62(t,
			raw_content.size(),
			x2_y2,
			sizeof(x2_y2),
			NULL,
			0,
			md)))
		{
			error_code = SM2Error::COMPUTE_SM2_KDF_FAIL;
			goto clean_up;
		}
		flag = 1;
		for (i = 0; i < raw_content.size(); i++)
		{
			if (t[i] != 0)
			{
				flag = 0;
				break;
			}
		}
	} while (flag);

	if (!(EC_POINT_get_affine_coordinates_GFp(group,
		c1_pt,
		bn_c1_x,
		bn_c1_y,
		ctx)))
	{
		goto clean_up;
	}

	if (BN_bn2binpad(bn_c1_x,
		c1_x,
		sizeof(c1_x)) != sizeof(c1_x))
	{
		goto clean_up;
	}
	if (BN_bn2binpad(bn_c1_y,
		c1_y,
		sizeof(c1_y)) != sizeof(c1_y))
	{
		goto clean_up;
	}
	c1_point[0] = 0x4;
	memcpy((c1_point + 1), c1_x, sizeof(c1_x));
	memcpy((c1_point + 1 + sizeof(c1_x)), c1_y, sizeof(c1_y));
	c1.append((char*)c1_point, sizeof(c1_point));

	EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, x2, sizeof(x2));
	EVP_DigestUpdate(md_ctx, raw_content.data(), raw_content.size());
	EVP_DigestUpdate(md_ctx, y2, sizeof(y2));
	EVP_DigestFinal_ex(md_ctx, (unsigned char*)(c3.data()), NULL);

	for (i = 0; i < raw_content.size(); i++)
	{
		c2[i] = raw_content[i] ^ t[i];
	}
	error_code = SM2Error::SUCCESS;

clean_up:
	if (t)
	{
		free(t);
	}
	if (ctx)
	{
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	if (group)
	{
		EC_GROUP_free(group);
	}

	if (pub_key_pt)
	{
		EC_POINT_free(pub_key_pt);
	}
	if (c1_pt)
	{
		EC_POINT_free(c1_pt);
	}
	if (s_pt)
	{
		EC_POINT_free(s_pt);
	}
	if (ec_pt)
	{
		EC_POINT_free(ec_pt);
	}
	if (md_ctx)
	{
		EVP_MD_CTX_free(md_ctx);
	}
	return error_code;
}
SM2EncryptMethodImp::SM2Error SM2EncryptMethodImp::nim_sm2_decrypt(const std::string&c1, const std::string&c2, const std::string&c3, 
	const std::string&pri_key, 
	std::string& decrypted_content)
{
	SM2Error error_code = SM2Error::SUCCESS;
	unsigned char c1_x[32], c1_y[32], x2[32], y2[32];
	unsigned char x2_y2[64], digest[32];
	unsigned char *t = NULL, *M = NULL;
	BN_CTX *ctx = NULL;
	BIGNUM *bn_d = NULL, *bn_c1_x = NULL, *bn_c1_y = NULL;
	BIGNUM *bn_x2 = NULL, *bn_y2 = NULL;
	const BIGNUM *bn_cofactor;
	EC_GROUP *group = NULL;
	EC_POINT *c1_pt = NULL, *s_pt = NULL, *ec_pt = NULL;
	const EVP_MD *md;
	EVP_MD_CTX *md_ctx = NULL;
	int message_len, i, flag;

	message_len = c2.size();
	memcpy(c1_x, (c1.data() + 1), sizeof(c1_x));
	memcpy(c1_y, (c1.data() + 1 + sizeof(c1_x)), sizeof(c1_y));

	error_code = SM2Error::ALLOCATION_MEMORY_FAIL;
	if (!(ctx = BN_CTX_new()))
	{
		goto clean_up;
	}
	BN_CTX_start(ctx);
	bn_d = BN_CTX_get(ctx);
	bn_c1_x = BN_CTX_get(ctx);
	bn_c1_y = BN_CTX_get(ctx);
	bn_x2 = BN_CTX_get(ctx);
	bn_y2 = BN_CTX_get(ctx);
	if (!(bn_y2))
	{
		goto clean_up;
	}
	if (!(group = EC_GROUP_new_by_curve_name(NID_sm2)))
	{
		goto clean_up;
	}

	if (!(c1_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}
	if (!(s_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}
	if (!(ec_pt = EC_POINT_new(group)))
	{
		goto clean_up;
	}

	if (!(md_ctx = EVP_MD_CTX_new()))
	{
		goto clean_up;
	}

	error_code = SM2Error::SM2_DECRYPT_FAIL;
	if (!(BN_bin2bn((unsigned char*)pri_key.data(), 32, bn_d)))
	{
		goto clean_up;
	}
	if (!(BN_bin2bn(c1_x, sizeof(c1_x), bn_c1_x)))
	{
		goto clean_up;
	}
	if (!(BN_bin2bn(c1_y, sizeof(c1_y), bn_c1_y)))
	{
		goto clean_up;
	}

	if (!(EC_POINT_set_affine_coordinates_GFp(group,
		c1_pt,
		bn_c1_x,
		bn_c1_y,
		ctx)))
	{
		goto clean_up;
	}
	if (EC_POINT_is_on_curve(group, c1_pt, ctx) != 1)
	{
		error_code = SM2Error::INVALID_SM2_CIPHERTEXT;
		goto clean_up;
	}

	if (!(bn_cofactor = EC_GROUP_get0_cofactor(group)))
	{
		goto clean_up;
	}
	if (!(EC_POINT_mul(group, s_pt, NULL, c1_pt, bn_cofactor, ctx)))
	{
		goto clean_up;
	}
	if (EC_POINT_is_at_infinity(group, s_pt))
	{
		error_code = SM2Error::INVALID_SM2_CIPHERTEXT;;
		goto clean_up;
	}

	if (!(EC_POINT_mul(group, ec_pt, NULL, c1_pt, bn_d, ctx)))
	{
		goto clean_up;
	}
	if (!(EC_POINT_get_affine_coordinates_GFp(group,
		ec_pt,
		bn_x2,
		bn_y2,
		ctx)))
	{
		goto clean_up;
	}
	if (BN_bn2binpad(bn_x2,
		x2,
		sizeof(x2)) != sizeof(x2))
	{
		goto clean_up;
	}
	if (BN_bn2binpad(bn_y2,
		y2,
		sizeof(x2)) != sizeof(y2))
	{
		goto clean_up;
	}
	memcpy(x2_y2, x2, sizeof(x2));
	memcpy((x2_y2 + sizeof(x2)), y2, sizeof(y2));
	md = EVP_sm3();

	if (!(t = (unsigned char *)malloc(message_len)))
	{
		goto clean_up;
	}
	if (!(ECDH_KDF_X9_62(t,
		message_len,
		x2_y2,
		sizeof(x2_y2),
		NULL,
		0,
		md)))
	{
		error_code = SM2Error::COMPUTE_SM2_KDF_FAIL;
		goto clean_up;
	}

	flag = 1;
	for (i = 0; i < message_len; i++)
	{
		if (t[i] != 0)
		{
			flag = 0;
			break;
		}
	}
	if (flag)
	{
		error_code = SM2Error::INVALID_SM2_CIPHERTEXT;
		goto clean_up;
	}

	if (!(M = (unsigned char *)malloc(message_len)))
	{
		goto clean_up;
	}
	for (i = 0; i < message_len; i++)
	{
		M[i] = c2[i] ^ t[i];
	}

	EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, x2, sizeof(x2));
	EVP_DigestUpdate(md_ctx, M, message_len);
	EVP_DigestUpdate(md_ctx, y2, sizeof(y2));
	EVP_DigestFinal_ex(md_ctx, digest, NULL);

	if (memcmp(digest, c3.data(), sizeof(digest)))
	{
		error_code = SM2Error::INVALID_SM2_CIPHERTEXT;
		goto clean_up;
	}
	memcpy(const_cast<char*>(decrypted_content.data()), M, message_len);
	error_code = SM2Error::SUCCESS;

clean_up:
	if (ctx)
	{
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	if (group)
	{
		EC_GROUP_free(group);
	}

	if (c1_pt)
	{
		EC_POINT_free(c1_pt);
	}
	if (s_pt)
	{
		EC_POINT_free(s_pt);
	}
	if (ec_pt)
	{
		EC_POINT_free(ec_pt);
	}

	if (md_ctx)
	{
		EVP_MD_CTX_free(md_ctx);
	}

	if (t)
	{
		free(t);
	}
	if (M)
	{
		free(M);
	}

	return error_code;
}
NIMENCRYPT_END_DECLS