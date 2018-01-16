#include "LuaProxy.h"
#include "UdpLog.h"
void init(const char *ip, int port, int logId){
	UdpLog::resetDst(ip, port, logId);
}
void log(int kind,const char *tag, const char *msg){
	switch (kind){
	case 0:
		UdpLog::i(tag, msg);
		break;
	case 1:
		UdpLog::d(tag, msg);
		break;
	case 2:
		UdpLog::w(tag, msg);
		break;
	case 3:
		UdpLog::e(tag, msg);
		break;
	}
}