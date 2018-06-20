#include "LuaProxy.h"
#include "UdpLog.h"

static int toLua_AppDelegate_UdpLogResetDst(lua_State* tolua_S) {
	const char* ip = lua_tostring(tolua_S, 1);
	int port = lua_tointeger(tolua_S, 2);
	int logid = lua_tointeger(tolua_S, 3);
	UdpLog::resetDst(ip, port, logid);
	return 0;
}
static int toLua_AppDelegate_UdpLog(lua_State* tolua_S) {
	int level = lua_tointeger(tolua_S, 1);
	const char *tag = lua_tostring(tolua_S, 2);
	const char *msg = lua_tostring(tolua_S, 3);
	switch (level) {
	case UDPLOG_LEVEL_I:
		UdpLog::i(tag, msg);
		break;
	case UDPLOG_LEVEL_D:
		UdpLog::d(tag, msg);
		break;
	case UDPLOG_LEVEL_W:
		UdpLog::w(tag, msg);
		break;
	case UDPLOG_LEVEL_E:
		UdpLog::e(tag, msg);
		break;
	}
	return 0;
}