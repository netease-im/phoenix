#include "nim_encrypt/encrypt/encrypt_utli.h"
NIMENCRYPT_BEGIN_DECLS

std::string EncryptUtli::BinaryToHexString(const std::string& binary)
{
	static const unsigned char kHexChars[] = "0123456789abcdef";
	std::string ret("");
	size_t length = binary.size();
	ret.resize(length << 1);
	if (length == 0)
		return ret;
	unsigned char *dst = reinterpret_cast<unsigned char *>(&ret[0]);
	const unsigned char *src = reinterpret_cast<const unsigned char *>(binary.data());

	for (size_t i = 0; i < length; i++)
	{
		dst[i << 1] = kHexChars[src[i] >> 4];
		dst[(i << 1) + 1] = kHexChars[src[i] & 0xF];
	}
	return ret;
}


NIMENCRYPT_END_DECLS