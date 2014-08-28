#include "uRobotics/utilities/Scanner.h"

using namespace Robotics::Utilities;

/************************************************************************/
/* Methods                                                              */
/************************************************************************/
bool Scanner::advanceToChar(const char& c, const std::string& input, size_t& cc)
{
	while (cc < input.length())
	{
		if(input[cc] == c)
			return true;
		++cc;
	}
	return false;
}

bool Scanner::advanceToChar(const std::vector<char>& c, const std::string& input, size_t& cc)
{
	while (cc < input.length())
	{	
		if(std::find(c.begin(), c.end(), input[cc]) != c.end())
			return true;
		++cc;
	}
	return false;
}

bool Scanner::readChar(const char& c, const std::string& input, size_t& cc)
{
	if ((cc < input.length()) && (input[cc] == c))
	{
		++cc;
		return true;
	}
	return false;
}

bool Scanner::readString(const std::string& expected, const std::string& input, size_t& cc)
{
	if (expected.length() == 0)
		return cc == input.length();

	size_t bcc = cc;
	for (size_t i = 0; i < expected.length(); ++i, ++bcc)
	{
		if (bcc >= input.length())
			return false;
		if (expected[i] != input[bcc])
			return false;
	}
	cc = bcc;
	return true;
}

void Scanner::skipSpaces(const std::string& s, size_t& cc)
{
	if ((cc < 0) || (cc >= s.length()))
		return;
	while (Scanner::isSpace(s[cc])) ++cc;
}

bool Scanner::xtractByte(const std::string& input, size_t& cc, unsigned char& value)
{
	size_t bcc = cc;
	int length;
	std::string sValue;
	int iValue;

	value = 0;
	while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 10))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> iValue;
	value = (unsigned char)iValue;
	return true;
}

bool Scanner::xtractSByte(const std::string& input, size_t& cc, signed char& value)
{
	size_t bcc = cc;
	int length;
	std::string sValue;
	int iValue;

	value = 0;
	if ((cc < input.length()) && (input[cc] == '-'))
		++cc;
	while ( (cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 4))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> iValue;
	value = (signed char)iValue;
	return true;
}

bool Scanner::xtractUInt16(const std::string& input, size_t& cc, unsigned short& value)
{
	size_t bcc = cc;
	int length;
	std::string sValue;

	value = 0;
	while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 5))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> value;
	return true;
}

bool Scanner::xtractInt16(const std::string& input, size_t& cc, short& value)
{
	size_t bcc = cc;
	int length;
	std::string sValue;

	value = 0;
	if ((cc < input.length()) && (input[cc] == '-'))
		++cc;
	while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 6))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> value;
	return true;
}

bool Scanner::xtractUInt32(const std::string& input, size_t& cc, unsigned int& value)
{
	size_t bcc = cc;
	int length;
	std::string sValue;

	value = 0;
	while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 10))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> value;
	return true;
}

bool Scanner::xtractInt32(const std::string& input, size_t& cc, int& value)
{
	size_t bcc = cc;
	int length;
	std::string sValue;

	value = 0;
	if ((cc < input.length()) && (input[cc] == '-'))
		++cc;
	while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 11))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> value;
	return true;
}

bool Scanner::xtractSingle(const std::string& input, size_t& cc, float& value){
	double dValue = 0;
	if(!Scanner::xtractDouble(input, cc, dValue))
		return false;
	value = (float)dValue;
	return true;
}

bool Scanner::xtractDouble(const std::string& input, size_t& cc, double& value)
{
	size_t bcc = cc;
	int dot;
	int length;
	std::string sValue;

	value = 0;
	// Sign
	if ((cc < input.length()) && (input[cc] == '-'))
		++cc;
	// Integer part
	while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		++cc;
	// Decimal part
	if ((cc < input.length()) && (input[cc] == '.'))
	{
		dot = cc;
		++cc;
		if ((cc >= input.length()) || !Scanner::isNumeric(input[cc]))
			return false;
		while ((cc < input.length()) && Scanner::isNumeric(input[cc]))
			++cc;
	}
	// Exponential part
	// The Exp e
	if ((cc < input.length()) && ((input[cc] == 'E') || (input[cc] == 'e')))
	{
		++cc;
		// The Exp sign (if any)
		if ((cc < input.length()) && ((input[cc] == '+') || (input[cc] == '-')))
			++cc;
		// First Exp digit
		if ((cc >= input.length()) || !Scanner::isNumeric(input[cc]))
			return false;
		++cc;
		// Second Exp digit (if any)
		if ((cc < input.length()) && Scanner::isNumeric(input[cc]))
		{
			++cc;
			// Third Exp digit (if any)
			if ((cc < input.length()) && Scanner::isNumeric(input[cc]))
				++cc;
		}

	}
	length = std::min((int)(cc - bcc), (int)(input.length() - bcc));
	if ((length < 1) || (length > 11))
		return false;
	sValue = input.substr(bcc, length);
	std::stringstream(sValue) >> value;
	return true;
}


bool Scanner::isAlNum(char c)
{
	return ((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

bool Scanner::isAlpha(char c)
{
	return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

bool Scanner::isLAlpha(char c)
{
	return ((c >= 'a') && (c <= 'z'));
}

bool Scanner::isLAlNum(char c)
{
	return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z'));
}

bool Scanner::isNumeric(char c)
{
	return ((c >= '0') && (c <= '9'));
}

bool Scanner::isSpace(char c)
{
	switch (c)
	{
		case ' ':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			return true;

		default:
			return false;

	}
}

bool Scanner::isUAlpha(char c)
{
	return ((c >= 'A') && (c <= 'Z'));
}

bool Scanner::isUAlNum(char c)
{
	return ((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'Z'));
}

