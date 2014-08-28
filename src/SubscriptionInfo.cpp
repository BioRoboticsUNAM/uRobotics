#include <uRobotics/api/SubscriptionInfo.h>
#include <uRobotics/Exception.h>
SubscriptionInfo::~SubscriptionInfo(void){}
/**
* Initializes a new instance of SubscriptionInfo
* Constructor provided for serialization purposes only
* @param variableInfo The object which contains information about the shared variable related to the subscription
*/
// Internal
SubscriptionInfo::SubscriptionInfo(ISharedVariableInfo& variableInfo) :
variableInfo(variableInfo), moduleName(std::string()),
subscriptionType(SharedVariableSubscriptionType::Unknown), reportType(SharedVariableReportType::Unknown),
writerModule(std::string()){}

/**
* Initializes a new instance of SubscriptionInfo
* @param variableInfo The object which contains information about the shared variable related to the subscription
* @param moduleName The name of the subscriber module
* @param subscriptionType The subscription type for the subscriber module
* @param reportType The report type for the subscription of the subscriber module
*/
SubscriptionInfo::SubscriptionInfo(ISharedVariableInfo& variableInfo, const std::string& moduleName,
								   const SharedVariableSubscriptionType& subscriptionType, const SharedVariableReportType& reportType):
variableInfo(variableInfo), moduleName(moduleName), subscriptionType(subscriptionType),
reportType(reportType), writerModule(std::string()){}

/**
* Initializes a new instance of SubscriptionInfo
* @param variableInfo The object which contains information about the shared variable related to the subscription
* @param moduleName The name of the subscriber module
* @param subscriptionType The subscription type for the subscriber module
* @param reportType The report type for the subscription of the subscriber module
* @param writerModule The name of the writer module for subscription types which allows to set an specific module
*/
SubscriptionInfo::SubscriptionInfo(ISharedVariableInfo& variableInfo, const std::string& moduleName,
								   const SharedVariableSubscriptionType& subscriptionType, const SharedVariableReportType& reportType, const std::string& writerModule):
variableInfo(variableInfo), moduleName(moduleName), subscriptionType(subscriptionType),
reportType(reportType), writerModule(writerModule){}

/**
* Gets or sets the name of the subscriber module
*/
std::string SubscriptionInfo::getModuleName() const { return moduleName; }

void SubscriptionInfo::setModuleName(const std::string& value){
	if (value.length() < 1)
		throw ArgumentNullException();
	moduleName = std::string(value);
}

/**
* Gets or sets the subscription type of the subscriber module
*/
SharedVariableSubscriptionType SubscriptionInfo::getSubscriptionType() const { return subscriptionType; }
void SubscriptionInfo::setSubscriptionType(const SharedVariableSubscriptionType& value){
	if ((int)value == SharedVariableSubscriptionType::Unknown)
		throw Exception("Invalid value");
	subscriptionType = SharedVariableSubscriptionType(value);
}

/**
* Gets or sets the report type for the subscription of the subscriber module
*/
SharedVariableReportType SubscriptionInfo::getReportType() const { return reportType; }
void SubscriptionInfo::setReportType(const SharedVariableReportType& value){
	if ((int)value == SharedVariableReportType::Unknown)
		throw Exception("Invalid value");
	reportType = SharedVariableReportType(value);
}

/**
* Gets or sets information about the shared variable related to the subscription
*/
ISharedVariableInfo& SubscriptionInfo::getVariableInfo() { return variableInfo; }
void SubscriptionInfo::setVariableInfo(ISharedVariableInfo& value){
	variableInfo = value;
}

/**
* Gets or sets the name of the writer module for subscription types which allows to set an specific module
*/
std::string SubscriptionInfo::getWriterModule() const { return writerModule; }
void SubscriptionInfo::setWriterModule(const std::string& value){
	if ((int)subscriptionType != SharedVariableSubscriptionType::WriteModule)
	{
		writerModule.clear();
		return;
	}
	else if (value.length() < 1)
		throw ArgumentNullException();
	writerModule = std::string(value);
}