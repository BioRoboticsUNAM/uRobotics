#include <uRobotics/api/SharedVariableReport.h>
#include <uRobotics/api/Parser.h>
#include <uRobotics/utilities/Scanner.h>
#include <uRobotics/utilities/Utils.h>

using namespace Robotics::Utilities;

SharedVariableReport::SharedVariableReport():
	subscriptionType(SharedVariableSubscriptionType::None),
	reportType(SharedVariableReportType::None),
	variableInfo(NULL){}

SharedVariableReport::~SharedVariableReport(){}

/**
* Initializes a new instance of SharedVariableReport
* @param variableInfo The information of the related shared variable
* @param serializedData The serialized value of the shared variable
* @param reportType The type of the report
* @param subscriptionType The type of the subscription
* @param writer  The name of the module which performed the write/create operation
*/
SharedVariableReport::SharedVariableReport(const ISharedVariableInfo* vInfo, const std::string& serializedData,
	const SharedVariableReportType& reportType, const SharedVariableSubscriptionType& subscriptionType, const std::string& writer):
	variableInfo(vInfo), serializedData(serializedData), reportType(reportType), subscriptionType(subscriptionType), writer(writer){}

/**
* Gets the type of the report
*/
const SharedVariableReportType SharedVariableReport::getReportType() const { return this->reportType; }

/**
* Gets the serialized data received with the report
*/
const std::string SharedVariableReport::getSerializedData() const { return this->serializedData; }

/**
* Gets the type of the subscription
*/
const SharedVariableSubscriptionType SharedVariableReport::getSubscriptionType() const { return this->subscriptionType; }

/**
* Gets the information of the related shared variable
*/
const ISharedVariableInfo* SharedVariableReport::getVariableInfo() const {
	return this->variableInfo;
}

/**
* Gets the name of the module which performed the write/create operation
*/
const std::string SharedVariableReport::getWriter() const { return this->writer; }


bool SharedVariableReport::getVariableInfo(std::string& parameters, SharedVariableInfo*& info, std::string& data, Exception& ex)
{
	std::string type;
	bool isArray;
	int size;
	std::string name;
	size_t cc;

	ex = Exception();
	data.clear();

	// 1. Get variable type
	cc = 0;
	if (!getVariableType(parameters, cc, type, isArray, size, ex))
		return false;

	// 2. Get variable name
	if (!getVariableName(parameters, cc, name, ex))
		return false;

	// 3. Get variable data
	if (!getVariableData(parameters, cc, data, ex))
		return false;

	info = new SharedVariableInfo(type, name, isArray, size);

	return true;	
}

bool SharedVariableReport::getWriter(std::string& parameters, std::string& writer, Exception& ex)
{
	unsigned int pos;

	writer.clear();
	ex = Exception();

	pos = parameters.rfind("%");
	
	if (pos == -1)
	{
		ex = Exception("Invalid parameters. No Writer found");
		return false;
	}
	writer = parameters.substr(pos + 1);
	parameters = parameters.substr(0, pos);
	Utils::trim(writer);
	Utils::trim(parameters);

	return true;
}

bool SharedVariableReport::getSubscriptionType(std::string& parameters, SharedVariableSubscriptionType& subscriptionType, Exception& ex)
{
	int pos;
	std::string sSubscriptionType;

	subscriptionType = SharedVariableSubscriptionType::WriteAny;
	ex = Exception();

	pos = parameters.rfind("%");
	if (pos == -1)
	{
		ex = Exception("Invalid parameters. No subscription type found.");
		return false;
	}
	sSubscriptionType = parameters.substr(pos + 1);
	parameters = parameters.substr(0, pos);
	Utils::trim(parameters);
	Utils::trim(sSubscriptionType);

	if(!SharedVariableSubscriptionType::tryParse(sSubscriptionType, subscriptionType))
	{
		ex = Exception("Invalid parameters. Invalid subscription type.");
		return false;
	}

	return true;
}

bool SharedVariableReport::getReportType(std::string& parameters, SharedVariableReportType& reportType, Exception& ex)
{
	int pos;
	std::string sReportType;

	reportType = SharedVariableReportType::Notify;

	pos = parameters.rfind('}');
	pos = std::max((int)parameters.rfind('%'), (int)parameters.rfind('}'));
	if (pos == -1)
	{
		ex = Exception("Invalid parameters. No report type type found.");
		return false;
	}
	sReportType = parameters.substr(pos + 1);
	Utils::trim(sReportType);
	parameters = parameters.substr(0, pos);
	Utils::trim(parameters);

	if(!SharedVariableReportType::tryParse(sReportType, reportType))
	{
		ex = Exception("Invalid parameters. Invalid report type.");
		return false;
	}
	
	return true;
}

bool SharedVariableReport::getVariableType(std::string& parameters, size_t& cc, std::string& type, bool& isArray, int& size, Exception& ex)
{
	unsigned short usize;

	type.clear();
	size = -1;
	isArray = false;
	ex = Exception();

	if (parameters[cc] == '{')
		parameters = parameters.substr(1, parameters.length() - 1);
	Utils::trim(parameters);
	if (!Parser::xtractIdentifier(parameters, cc, type))
	{
		ex = Exception("Invalid parameters. No variable type found.");
		return false;
	}
	isArray = false;
	size = -1;
	if (Scanner::readChar('[', parameters, cc))
	{
		if (Scanner::xtractUInt16(parameters, cc, usize))
			size = usize;
		if (!Scanner::readChar(']', parameters, cc))
		{
			ex = Exception("Invalid parameters. Expected ']'.");
			return false;
		}
		isArray = true;
	}

	return true;
}

bool SharedVariableReport::getVariableName(std::string& parameters, size_t& cc, std::string& name, Exception& ex)
{
	ex = Exception();
	name.clear();

	Scanner::skipSpaces(parameters, cc);
	if (!Parser::xtractIdentifier(parameters, cc, name))
	{
		ex = Exception("Invalid parameters. Expected identifier.");
		return false;
	}
	return true;	
}

bool SharedVariableReport::getVariableData(std::string& parameters, size_t& cc, std::string& data, Exception& ex)
{
	ex = Exception();
	Scanner::skipSpaces(parameters, cc);
	data = parameters.substr(cc);
	return true;
}

/**
* Generates a SharedVariableReport object from a Response object
* @param response The Response object to be used to generate the report
* @return A SharedVariableReport object created from the Response object
*/
SharedVariableReport* SharedVariableReport::createFromResponse(const Response& response)
{
	Exception ex;
	SharedVariableReport *report;
	if (!createFromResponse(response, report, ex))
		throw ex;
	return report;

}
/**
* Generates a SharedVariableReport object from a Response object
* @param response The Response object to be used to generate the report
* @param report When this method returns contains the SharedVariableReport object
* extracted from the response if the parse operation was completed successfully, NULL otherwise
* @param ex When this method returns contains NULL if the parse operation was completed successfully,
* or the exception to be thrown if the operation failed
* @return A SharedVariableReport object created from the Response object
*/
//internal
bool SharedVariableReport::createFromResponse(const Response& response, SharedVariableReport*& report){
	Exception ex;
	return SharedVariableReport::createFromResponse(response, report, ex);
}

/**
* Generates a SharedVariableReport object from a Response object
* @param response The Response object to be used to generate the report
* @param report When this method returns contains the SharedVariableReport object
* extracted from the response if the parse operation was completed successfully, NULL otherwise
* @param ex When this method returns contains NULL if the parse operation was completed successfully,
* or the exception to be thrown if the operation failed
* @return A SharedVariableReport object created from the Response object
*/
//internal
bool SharedVariableReport::createFromResponse(const Response& response, SharedVariableReport*& report, Exception& ex)
{
	std::string parameters;
	std::string data;

	std::string writer;
	SharedVariableReportType reportType = SharedVariableReportType::None;
	SharedVariableSubscriptionType subscriptionType = SharedVariableSubscriptionType::None;
	SharedVariableInfo* varInfo;

	report = NULL;
	ex = Exception();

	parameters = response.parameters;
	// 1. Get writer
	if (!getWriter(parameters, writer, ex))
		return false;

	// 2. Get subscription type
	if(!getSubscriptionType(parameters, subscriptionType, ex))
		return false;

	// 3. Get report type.
	if (!getReportType(parameters, reportType, ex))
		return false;

	// 4. Get variable info
	if (!getVariableInfo(parameters, varInfo, data, ex))
		return false;

	// 5. Create the report
	report = new SharedVariableReport(varInfo, data, reportType, subscriptionType, writer);
	
	return true;
}