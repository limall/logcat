#ifndef UDPLOG_H
#define UDPLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <string>

using  namespace std;

class UdpLog{
private:
	static const int BLOCK_LEN = 4096;
	static const int MASTHEAD_HEADOFLOG = 0;
	static const int MASTHEAD_BODYOFLOG = 1;
	static const int MASTHEAD_RESETLOGID = 2;

	static int _id;
	static int _socket;
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
	static void e(string tag,string msg);
	static void d(string tag, string msg);
	static void w(string tag, string msg);
};
#endif

