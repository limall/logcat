#include "LuaProxy.h"
#include "UdpLog.h"
void init(const char *ip, int port, int logId){
	UdpLog::resetDst(ip, port, logId);
}
void i(const char *tag, const char *msg){
	UdpLog::i(tag, msg);
}
void d(const char *tag, const char *msg){
	UdpLog::d(tag, msg);
}
void e(const char *tag, const char *msg){
	UdpLog::e(tag, msg);
}
void w(const char *tag, const char *msg){
	UdpLog::w(tag, msg);
}