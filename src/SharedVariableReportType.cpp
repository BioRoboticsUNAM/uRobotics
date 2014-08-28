#include <uRobotics/api/SharedVariableReportType.h>

SharedVariableReportType::SharedVariableReportType():Enum(0){}
SharedVariableReportType::SharedVariableReportType(int v) : Enum(((v < -1) || (v > 2)) ? -1 : v){
}
SharedVariableReportType::SharedVariableReportType(const SharedVariableReportType& svrt):Enum((int)svrt){}
SharedVariableReportType::~SharedVariableReportType(){}
const bool SharedVariableReportType::tryParse(const std::string& s, SharedVariableReportType& type){
	if(s.compare("None") == 0){
		type = SharedVariableReportType(None);
		return true;
	}
	else if(s.compare("Notify") == 0){
		type = SharedVariableReportType(Notify);
		return true;
	}
	else if(s.compare("SendContent") == 0){
		type = SharedVariableReportType(SendContent);
		return true;
	}
	type = SharedVariableReportType(Unknown);
	return false;
}