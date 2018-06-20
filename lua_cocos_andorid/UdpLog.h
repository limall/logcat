#pragma once
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <mutex>
#ifdef _WIN32
#include <dbghelp.h>
#include <string>
#include <sstream>
#pragma comment (lib, "dbghelp.lib")  
#endif  // _WIN32
//e,w,i,d
using namespace std;
class UdpLog{
private:
	static const int BLOCK_LEN = 4096;
	static const int MASTHEAD_HEADOFLOG = 0;
	static const int MASTHEAD_BODYOFLOG = 1;
	static const int MASTHEAD_RESETLOGID = 2;

	static std::mutex *_mt;
	static int _id;
	static SOCKET _socket;
	static struct sockaddr * _sockaddr;
	static int _srvAddrLen;
	static int _logId;

	static int getSingleId();
	static void sendBlock(const char *block, bool isBegin, int id, int totoallength,int order);
	static void sendMsg(const char *msg);
public:
	static void setDst(const char *ip, int port,int logId);
	static void resetDst(const char *ip, int port, int logId);
	static void i(string tag,string msg);
	static void d(string tag, string msg);
	static void w(string tag, string msg);
	static void e(string tag,string msg);
};

