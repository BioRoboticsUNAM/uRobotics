#include "uRobotics/api/Parser.h"

using namespace Robotics::Utilities;

Parser::Parser(void)
{
}

Parser::~Parser(void)
{
}

/**
* Decodes the data contained in a received response. A return value indicates whether the operation succeeded
* @param s	std::string which contains the data to parse
* @param variableType	When this method returns contains the type of the variable coded in the input std::string if the conversion succeeded, or null if the conversion failed.
* @param isArray	When this method returns is set to true if the conversion succeded and the variable coded in the input std::string is an array, false otherwise.
* @param arrayLength	When this method returns contains the size of the array if the conversion succeded and the variable coded in the input std::string is an array, -1 otherwise.
* @param variableName	When this method returns contains the name of the variable coded in the input std::string if the conversion succeeded, or null if the conversion failed.
* @param variableData	When this method returns contains the content of the variable coded in the input std::string if the conversion succeeded, or the null if the conversion failed.
* @return true if the the data extraction succeeded, false otherwise
*/
bool Parser::parseSharedVariable(const std::string& s, std::string& variableType, bool& isArray, int& arrayLength, std::string& variableName, std::string& variableData)
{
	size_t cc;
	int end;
	int dataSize;
	std::string id1;
	std::string id2;
	unsigned short arraySize;
	bool isSizedArray;

	// Initialize variables
	cc = 0;
	isSizedArray = false;
	variableType.clear();
	isArray = false;
	arrayLength = -1;
	variableName.clear();
	variableData.clear();

	// Empty std::string contains no variable data. Return.
	if (s.empty()) return false;

	// Check if the std::string is enclosed within braces
	end = s.length() - 1;
	if (Scanner::readChar('{', s, cc))
	{
		if (s[end] != '}')
			return false;
		--end;
		while (Scanner::isSpace(s[end]))
			--end;
		++end;
	}

	// Skip space characters at the beginning
	skipSpaces(s, cc);

	// Read the variable type
	if (!xtractIdentifier(s, cc, id1))
		return false;

	// Check if variable is an array and get the array length
	arraySize = 0;
	if ((cc < s.length()) && (s[cc++] == '['))
	{
		// Skip space characters between square bracket and array size (if any)
		skipSpaces(s, cc);
		// Read array size (if any)
		if (Scanner::xtractUInt16(s, cc, arraySize))
			isSizedArray = true;
		// Skip space characters between and array size (if any) and square bracket
		skipSpaces(s, cc);
		if ((cc >= s.length()) || (s[cc++] != ']'))
			return false;
		isArray = true;
	}

	// Skip space characters between variable type and it's name
	skipSpaces(s, cc);

	// Read the variable name
	if (!xtractIdentifier(s, cc, id2))
		return false;

	// Skip space characters between variable name and variable content
	skipSpaces(s, cc);

	// Calculate data size
	dataSize = 1 + end - cc;
	if (dataSize < 0)
		return false;

	// Set values
	variableType = id1;
	variableName = id2;
	arrayLength = (isArray && isSizedArray) ? arraySize : -1;
	if(dataSize < 1)variableData.clear();
	else variableData = s.substr(cc, dataSize);
	return true;
}

/**
* Extracts the type, name and data from a shared variable
* @param s	String from which the variable will be extracted
* @param cc	The search starting position
* @param varType	When this method returns contains the type of the variable found in s if the extraction succeded, or null if the extraction failed.
* @param varName	When this method returns contains the name of the variable found in s if the extraction succeded, or null if the extraction failed.
* @param isArray	When this method returns is set to true if a variable was found in s and it is an array, or false otherwise
* @param arraySize	When this method returns contains the length of the array of the variable found in s if the extraction succeded and the variable is an array, or -1 otherwise
* @return true if the extraction succeded, false otherwise
*/
//[Obsolete]
bool Parser::xtractSharedVariableData(const std::string& s, size_t& cc, std::string& varType, std::string& varName, bool& isArray, int& arraySize)
{
	std::string id1;
	std::string id2;
	unsigned short size;
	bool isSizedArray = false;

	varType.clear();
	varName.clear();
	isArray = false;
	arraySize = -1;

	skipSpaces(s, cc);
	if (!xtractIdentifier(s, cc, id1))
		return false;
	skipSpaces(s, cc);

	// Array
	size = 0;
	if ((cc < s.length()) && (s[cc] == '['))
	{
		skipSpaces(s, cc);
		if (Scanner::xtractUInt16(s, cc, size))
			isSizedArray = true;
		skipSpaces(s, cc);
		if ((cc >= s.length()) || (s[cc] != ']'))
			return false;
		isArray = true;
	}

	skipSpaces(s, cc);
	if (!xtractIdentifier(s, cc, id2))
		return false;

	varType = id1;
	varName = id2;
	arraySize = (isArray && isSizedArray) ? size : -1;
	return true;
}

/**
* Extracts the type, name and data from a shared variable
* @param s	String from which the variable will be extracted
* @param cc	The search starting position
* @param varType	When this method returns contains the type of the variable found in s if the extraction succeded, or null if the extraction failed.
* @param varName	When this method returns contains the name of the variable found in s if the extraction succeded, or null if the extraction failed.
* @param isArray	When this method returns is set to true if a variable was found in s and it is an array, or false otherwise
* @param arraySize	When this method returns contains the length of the array of the variable found in s if the extraction succeded and the variable is an array, or -1 otherwise
* @param data	When this method returns contains the data of the variable found in s if the extraction succeded, or null if the extraction failed.
* @return true if the extraction succeded, false otherwise
*/
//[Obsolete]
bool Parser::xtractSharedVariableData(const std::string& s, size_t& cc, std::string& varType, std::string& varName, bool& isArray, int& arraySize, std::string& data)
{
	varType.clear();
	varName.clear();
	isArray = false;
	arraySize = -1;
	data.clear();

	if(!xtractSharedVariableData(s, cc, varType, varName, isArray, arraySize))
		return false;
	if (cc >= s.length() - 1)
		data.clear();
	else
	{
		data = s.substr(cc);
		cc = s.length();
	}
	return true;
}

/**
* Extracts the first module name found inside a std::string
* @param s	String from which the module name will be extracted
* @param cc	The search starting position
* @param moduleName	When this method returns contains the first module name found in s if the extraction succeded, or null if the extraction failed.
* @return True if the extraction succeded and a valid module name was found in s starting at cc, false otherwise
*/
bool Parser::xtractModuleName(const std::string& s, size_t& cc, std::string& moduleName)
{
	int bcc = cc;
	int length;
	moduleName.clear();

	if ((cc < 0) || (cc >= s.length()) || (s[cc] == 0) || (s[cc] > 128))
		return false;

	if (!Scanner::isUAlpha(s[cc])) return false;
	++cc;
	while ((cc < s.length()) && ((Scanner::isUAlNum(s[cc]) || s[cc] == '-')))
		++cc;
	length = std::min(cc - bcc, s.length() - bcc);
	moduleName = s.substr(bcc, length);
	return true;
}

/**
* Extracts the first command name found inside a std::string
* @param s	String from which the command name will be extracted
* @param cc	The search starting position
* @param commandName	When this method returns contains the first command name found in s if the extraction succeded, or null if the extraction failed.
* @return True if the extraction succeded and a valid command name was found in s starting at cc, false otherwise
*/
bool Parser::xtractCommandName(const std::string& s, size_t& cc, std::string& commandName)
{
	int bcc = cc;
	int length;
	commandName.clear();

	if ((cc < 0) || (cc >= s.length()) || (s[cc] == 0) || (s[cc] > 128))
		return false;

	if (!Scanner::isLAlpha(s[cc]) && (s[cc] != '_')) return false;
	++cc;
	while ((cc < s.length()) && ((Scanner::isLAlNum(s[cc]) || s[cc] == '_')))
		++cc;
	length = std::min(cc - bcc, s.length() - bcc);
	commandName = s.substr(bcc, length);
	return true;
}

/**
* Advances the read header until no spaces are found
* @param s	Input std::string
* @param cc	Read header
*/
void Parser::skipSpaces(const std::string& s, size_t& cc)
{
	if ((cc < 0) || (cc >= s.length()))
		return;
	while (Scanner::isSpace(s[cc])) ++cc;
}

/**
* Extracts the first command parameters found inside a std::string
* @param s	String from which the command parameters will be extracted
* @param cc	The search starting position
* @param parameters	When this method returns contains the first command parameters found in s if the extraction succeded, or null if the extraction failed.
* @return True if the extraction succeded and a valid command parameters was found in s starting at cc, false otherwise
*/
bool Parser::xtractCommandParams(const std::string& s, size_t& cc, std::string& parameters)
{
	int bcc = cc + 1;
	int length;
	parameters.clear();

	if ((cc < 0) || (cc >= s.length()) || (s[cc] == 0) || (s[cc] > 128))
		return false;

	if (s[cc] != '"') return false;
	++cc;
	while ((cc < s.length()) && (s[cc] != '"') && (s[cc] != 0) && (s[cc] < 128))
	{
		if (s[cc] == '\\')
			++cc;
		++cc;
	}
	length = std::min(cc - bcc, s.length() - bcc);
	if ( (cc >= s.length()) || (s[cc] != '"') ) return false;
	++cc;
	if (length < 0)
		return false;
	parameters = s.substr(bcc, length);
	return true;
}

/**
* Extracts the first @id found inside a std::string
* @param s	String from which the @id will be extracted
* @param cc	The search starting position
* @param id	When this method returns contains the id found in s if the extraction succeded, or -1 if the extraction failed.
* @return True if the extraction succeded and a valid id was found in s starting at cc, false otherwise
*/
bool Parser::xtractId(const std::string& s, size_t& cc, int& id)
{
	int bcc = cc + 1;
	int length;
	std::string sid;
	id = -1;

	if ((cc < 0) || (cc >= s.length()) || (s[cc] == 0) || (s[cc] > 128))
		return false;

	if (s[cc] != '@') return false;
	++cc;

	while ((cc < s.length()) && Scanner::isNumeric(s[cc])) ++cc;
	length = std::min(cc - bcc, s.length() - bcc);
	sid = s.substr(bcc, length);
	std::stringstream(sid) >> id;
	return true;
}

/**
* Extracts the first C-type identifier found inside a std::string
* @param s	String from which the identifier will be extracted
* @param cc	The search starting position
* @param identifier	When this method returns contains the first C-type identifier found in s if the extraction succeded, or null if the extraction failed.
* @return True if the extraction succeded and a valid C-type identifier was found in s starting at cc, false otherwise
*/
bool Parser::xtractIdentifier(const std::string& s, size_t& cc, std::string& identifier)
{
	int bcc = cc;
	int length;
	identifier.clear();

	if ((cc < 0) || (cc >= s.length()) || (s[cc] == 0) || (s[cc] > 128))
		return false;

	if (!Scanner::isAlpha(s[cc]) && (s[cc] != '_')) return false;
	++cc;
	while ((cc < s.length()) && (Scanner::isAlNum(s[cc]) || (s[cc] == '_')))
		++cc;
	length = std::min(cc - bcc, s.length() - bcc);
	identifier = s.substr(bcc, length);
	return true;
}

/**
* Extracts the first result (1 or 0) found inside a std::string
* @param s	String from which the result will be extracted
* @param cc	The search starting position
* @param result	When this method returns contains the result found in s if the extraction succeded, or -1 if the extraction failed.
* @return True if the extraction succeded and a valid result was found in s starting at cc, false otherwise
*/
bool Parser::xtractResult(const std::string& s, size_t& cc, int& result)
{
	int ncc = cc + 1;
	result = -1;

	if ((cc < 1) || (cc >= s.length()) || (s[cc] == 0) || (s[cc] > 128))
		return false;

	if (
		Scanner::isSpace(s[cc - 1]) &&
		((s[cc] == '1') || (s[cc] == '0')) &&
		((ncc == s.length()) || (s[ncc] == 0) || Scanner::isSpace(s[ncc]))
		)
	{
		result = s[cc] - '0';
		return true;
	}
	return false;
}