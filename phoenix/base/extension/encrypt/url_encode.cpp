#include "extension/encrypt/url_encode.h"
#include <assert.h>

EXTENSION_BEGIN_DECLS

unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') 
		y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') 
		y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') 
		y = x - '0';
	else 
		assert(0);
	return y;
}

std::string URLEncode(const std::string& input)
{
	std::string output;
	size_t length = input.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)input[i]) ||
			(input[i] == '-') ||
			(input[i] == '_') ||
			(input[i] == '.') ||
			(input[i] == '~')) {
			output += input[i];
		}
		else if (input[i] == ' '){
			output += "+";
		}
		else
		{
			output += '%';
			output += ToHex((unsigned char)input[i] >> 4);
			output += ToHex((unsigned char)input[i] % 16);
		}
	}
	return output;
}
std::string URLDecode(const std::string& input)
{
	std::string output;
	size_t length = input.length();
	for (size_t i = 0; i < length; i++)
	{
		if (input[i] == '+') {
			output += ' ';
		}
		else if (input[i] == '%'){
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)input[++i]);
			unsigned char low = FromHex((unsigned char)input[++i]);
			output += high * 16 + low;
		}
		else{ 
			output += input[i];
		}
	}
	return output;
}

EXTENSION_END_DECLS