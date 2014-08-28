#include <uRobotics/api/SharedVariableSubscriptionType.h>

SharedVariableSubscriptionType::SharedVariableSubscriptionType():Enum(0){}
SharedVariableSubscriptionType::SharedVariableSubscriptionType(int v): Enum(((v < -1) || (v > 4)) ? -1 : v){}
SharedVariableSubscriptionType::SharedVariableSubscriptionType(const SharedVariableSubscriptionType& svst):Enum((int)svst){}
SharedVariableSubscriptionType::~SharedVariableSubscriptionType(){}
const bool SharedVariableSubscriptionType::tryParse(const std::string& s, SharedVariableSubscriptionType& type){
	if(s.compare("None") == 0){
		type = SharedVariableSubscriptionType(None);
		return true;
	}
	else if(s.compare("Creation") == 0){
		type = SharedVariableSubscriptionType(Creation);
		return true;
	}
	else if(s.compare("WriteAny") == 0){
		type = SharedVariableSubscriptionType(WriteAny);
		return true;
	}
	else if(s.compare("WriteOthers") == 0){
		type = SharedVariableSubscriptionType(WriteOthers);
		return true;
	}
	else if(s.compare("WriteModule") == 0){
		type = SharedVariableSubscriptionType(WriteModule);
		return true;
	}
	type = SharedVariableSubscriptionType(Unknown);
	return false;
}