#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <uRobotics/api/SharedVariableInfo.h>
#include <uRobotics/api/Parser.h>
#include <uRobotics/utilities/Scanner.h>

using namespace Robotics::Utilities;

SharedVariableInfo::~SharedVariableInfo(void){}
SharedVariableInfo::SharedVariableInfo(void){}

SharedVariableInfo::SharedVariableInfo(const SharedVariableInfo& other):
typeName(other.typeName), name(other.name), isArray_(other.isArray_), length(other.length){}

SharedVariableInfo::SharedVariableInfo(const std::string& typeName, const std::string& name):
typeName(typeName), name(name), isArray_(false), length(-1){}

SharedVariableInfo::SharedVariableInfo(const std::string& typeName, const std::string& name, const bool& isArray, const int& length):
typeName(typeName), name(name), isArray_(false), length(-1){
	this->length = (this->isArray_ = isArray) ? length : -1;
}

const SharedVariableInfo SharedVariableInfo::None("(none)", "(none)");

/**
Gets a value indicating if the variable is an array
*/
const bool SharedVariableInfo::isArray() const{
	return isArray_;
}

/**
If the variable is an array gets the length of the array, else returns -1
*/
const int SharedVariableInfo::getLength() const{
	return length;
}

/**
Gets the name of the SharedVariable
*/
const std::string SharedVariableInfo::getName() const{
	return name;
}

/**
Gets the global name of the type of the SharedVariable
*/
const std::string SharedVariableInfo::getTypeName() const{
	return typeName;
}

bool SharedVariableInfo::deserialize(const std::string& serialized, SharedVariableInfo& info){
	Exception ex;
	return deserialize(serialized, info, ex);
}

bool SharedVariableInfo::deserialize(const std::string& serialized, SharedVariableInfo& info, Exception& ex)
{
	size_t cc = 0;
	std::vector<std::string> writers;
	std::vector<SubscriptionInfo*> subscriptions;
	boost::posix_time::ptime creationTime;

	if (serialized.length() < 1)
	{
		ex = Exception("Invalid input string");
		return false;
	}

	// 1. Deserialize variable information
	if (!deserializeSVInfo(serialized, cc, info, ex)){
		return false;
	}

	if (!deserializeCreationTime(serialized, cc, creationTime, ex)){
		return false;
	}

	// 2. Deserialize writers
	if (!deserializeWriters(serialized, cc, writers, ex)){
		return false;
	}

	// 3. Deserialize subscriptions information
	if (!deserializeSubscriptions(info, serialized, cc, subscriptions, ex)){
		return false;
	}

	// 4. Integrate package
	//info.creationTime = creationTime;
	//info.subscriptions = subscriptions;
	//info.allowedWriters = writers;
	return true;
}

bool SharedVariableInfo::deserializeSVInfo(const std::string& serialized, size_t& cc, SharedVariableInfo& info, Exception& ex){
	std::string variableType;
	std::string variableName;
	unsigned short uLength;
	size_t length = -1;
	bool isArray = false;

	// 1. Initialize output values

	// 2. Extract variable type
	if (!Parser::xtractIdentifier(serialized, cc, variableType)){
		ex = Exception("Expected identifier (variable type)");
		return false;
	}

	// 3. Get variable array data
	if (Scanner::readChar('[', serialized, cc)){
		length = Scanner::xtractUInt16(serialized, cc, uLength) ? uLength : -1;
		if (!Scanner::readChar(']', serialized, cc)){
			ex = Exception("Expected ']'");
			return false;
		}
		isArray = true;
	}

	// 4. Get variable name
	Parser::skipSpaces(serialized, cc);
	if (!Parser::xtractIdentifier(serialized, cc, variableName)){
		ex = Exception("Expected identifier (variable name)");
		return false;
	}

	info = SharedVariableInfo(variableType, variableName, isArray, length);
	return true;
}

bool SharedVariableInfo::deserializeCreationTime(const std::string& serialized, size_t& cc, boost::posix_time::ptime& creationTime, Exception& ex){
	size_t bcc;

	//creationTime = DateTime.MinValue;
	//ex = null;
	Scanner::skipSpaces(serialized, cc);
	if (!Scanner::readString("creationTime=", serialized, cc))
		return true;

	if (!Scanner::readChar('{', serialized, cc)){
		ex = Exception("Invalid creationTime segment, expected '{'");
		return false;
	}

	bcc = cc;
	// Read until next '}' character
	while (cc < serialized.length()){
		if (serialized[cc] == '}')
			break;
		++cc;
	}
	if (cc >= serialized.length()){
		ex = Exception("Invalid creationTime segment, expected '}'");
		return false;
	}

	if (cc == bcc){
		ex = Exception("Invalid creation Time. Incorrect format.");
		return false;
	}
	std::istringstream iss(serialized.substr(bcc, cc - bcc));
	iss >> creationTime;

	// Finally skip the closing }
	Scanner::readChar('}', serialized, cc);
	return true;
}

bool SharedVariableInfo::deserializeWriters(const std::string& serialized, size_t& cc, std::vector<std::string>& writers, Exception& ex){
	size_t bcc;
	writers.clear();

	Scanner::skipSpaces(serialized, cc);
	if (!Scanner::readString("writers=", serialized, cc))
		return true;

	if (!Scanner::readChar('{', serialized, cc))
	{
		ex = Exception("Invalid writers segment, expected '{'");
		return false;
	}

	bcc = cc;
	// Read until next '}' character
	while (cc < serialized.length())
	{
		if (serialized[cc] == '}')
			break;
		++cc;
	}
	if (cc >= serialized.length())
	{
		ex = Exception("Invalid writers segment, expected '}'");
		return false;
	}

	if (cc == bcc)
	{
		ex = Exception("Invalid writers segment. Incorrect format.");
		return false;
	}
	std::string segment = serialized.substr(bcc, cc - bcc);
	boost::split(writers, segment, boost::is_any_of(","));
	if ((writers.size() == 1) && (writers[0] == "*"))
		writers.clear();

	// Finally skip the closing }
	Scanner::readChar('}', serialized, cc);
	return true;

}

bool SharedVariableInfo::deserializeSubscriptions(SharedVariableInfo svInfo, const std::string& serialized, size_t& cc, std::vector<SubscriptionInfo*>& subscriptions, Exception& ex)
{
	subscriptions.clear();
	Scanner::skipSpaces(serialized, cc);
	if (!Scanner::readString("subscriptions=", serialized, cc))
		return true;

	if (!Scanner::readChar('{', serialized, cc)){
		ex = Exception("Invalid subscriptions segment, expected '{'");
		return false;
	}

	while ((cc < serialized.length()) && (serialized[cc] == '{')){
		SubscriptionInfo *subscription = NULL;
		if (!deserializeSubscription(svInfo, serialized, cc, subscription, ex))
			return false;
		subscriptions.push_back(subscription);
	}

	if (!Scanner::readChar('}', serialized, cc))
	{
		for(size_t i = 0; i < subscriptions.size(); ++i)
			delete subscriptions[i];
		subscriptions.clear();
		ex = Exception("Invalid subscriptions segment, expected '}'");
		return false;
	}

	return true;
}

bool SharedVariableInfo::deserializeSubscription(SharedVariableInfo svInfo, const std::string& serialized, size_t& cc, SubscriptionInfo*& subscription,  Exception& ex)
{
	//int bcc;
	std::string subscriber;
	SharedVariableSubscriptionType sType(SharedVariableSubscriptionType::Unknown);
	SharedVariableReportType rType(SharedVariableReportType::Unknown);
	std::string writer;

	subscription = NULL;
	if (!Scanner::readChar('{', serialized, cc)){
		ex = Exception("Invalid subscriptions segment, expected '{'");
		return false;
	}

	if (!deserializeSubscriber(serialized, cc, subscriber, ex))
		return false;
	if (!deserializeSubscriptionType(serialized, cc, sType, ex))
		return false;
	if (!deserializeReportType(serialized, cc, rType, ex))
		return false;
	if (((int)sType == SharedVariableSubscriptionType::WriteModule) && !deserializeSubscriptionWriter(serialized, cc, writer, ex))
		return false;

	if (!Scanner::readChar('}', serialized, cc)){
		ex = Exception("Invalid subscriptions segment, expected '}'");
		return false;
	}

	subscription = new SubscriptionInfo(svInfo, subscriber, sType, rType, writer);
	return true;
}

bool SharedVariableInfo::deserializeSubscriber(const std::string& serialized, size_t& cc, std::string& subscriber, Exception& ex)
{
	size_t bcc;

	subscriber.clear();
	if (!Scanner::readString("subscriber=", serialized, cc))
	{
		ex = Exception("Invalid subscriptions segment, expected 'subscriber='");
		return false;
	}
	bcc = cc;
	if (!Scanner::advanceToChar(',', serialized, cc))
	{
		ex = Exception("Invalid subscriptions segment, expected ','");
		return false;
	}
	subscriber = serialized.substr(bcc, cc - bcc);
	return true;
}

bool SharedVariableInfo::deserializeSubscriptionType(const std::string& serialized, size_t& cc, SharedVariableSubscriptionType& sType, Exception& ex)
{
	size_t bcc;

	sType = SharedVariableSubscriptionType::Unknown;
	if (!Scanner::readString(", sType=", serialized, cc)){
		ex = Exception("Invalid subscriptions segment (subscription type), expected ','");
		return false;
	}
	bcc = cc;
	if (!Scanner::advanceToChar(',', serialized, cc)){
		ex = Exception("Invalid subscriptions segment (subscription type), expected ','");
		return false;
	}

	if(!SharedVariableSubscriptionType::tryParse(serialized.substr(bcc, cc - bcc), sType)){
		ex = Exception("Invalid value for subscription type");
		return false;
	}
	return true;
}

bool SharedVariableInfo::deserializeReportType(const std::string& serialized, size_t& cc, SharedVariableReportType& rType, Exception& ex)
{
	size_t bcc;

	rType = SharedVariableReportType::Unknown;
	if (!Scanner::readString(", rType=", serialized, cc))
	{
		ex = Exception("Invalid subscriptions segment (subscription report type), expected ','");
		return false;
	}
	bcc = cc;
	std::vector<char> range;
	range.push_back(',');
	range.push_back('}');
	if (!Scanner::advanceToChar(range, serialized, cc)){
		ex = Exception("Invalid subscriptions segment (subscription report type), expected '}'");
		return false;
	}

	if(!SharedVariableReportType::tryParse(serialized.substr(bcc, cc - bcc), rType)){
		ex = Exception("Invalid value for subscription report type");
		return false;
	}
	return true;
}

bool SharedVariableInfo::deserializeSubscriptionWriter(const std::string& serialized, size_t& cc, std::string& subscriber, Exception& ex){
	int bcc;

	subscriber.clear();
	if (!Scanner::readString("writer=", serialized, cc))
	{
		ex = Exception("Invalid subscriptions segment, expected 'writer='");
		return false;
	}
	bcc = cc;
	if (!Scanner::advanceToChar('}', serialized, cc)){
		ex = Exception("Invalid subscriptions segment, expected ','");
		return false;
	}
	subscriber = serialized.substr(bcc, cc - bcc);
	return true;
}
