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
class UdpLog{
private:
	static const int BLOCK_LEN = 128;
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
	//前8位为是否是开头，接下来就是32位logid,接下来是32位id，接下来是16位总长度，接下来是8位order,接下来是128byte的数据。
	static void sendBlock(const char *block, bool isBegin, int id, int totoallength,int order);
	static void sendMsg(const char *msg);
	static std::string format(char *fmt, va_list ap);
	std::string _tag;

public:
	static void setDst(const char *ip, int port,int logId);
	static void resetDst(const char *ip, int port, int logId);
	static void i(const char *tag, const char *msg);
	static void d(const char *tag, const char *msg);
	static void e(const char *tag, const char *msg);
	static void w(const char *tag, const char *msg);
	static void si(char *fmt,...);
	static void sd(char *fmt,...);
	static void se(char *fmt,...);
	static void sw(char *fmt,...);
	static void iBytes(const char *tag,unsigned char *bytes, int len);
	static void iStackTrace(char *tag,...);
	static void dStackTrace(char *tag, ...);
	static void wStackTrace(char *tag, ...);
	static void eStackTrace(char *tag, ...);
	static UdpLog *Tag(char *fmt, ...);
	void logi(char *fmt, ...);
	void logd(char *fmt, ...);
	void logw(char *fmt, ...);
	void loge(char *fmt, ...);
};

