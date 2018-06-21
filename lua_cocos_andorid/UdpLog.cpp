#include "UdpLog.h"

using namespace std;

typedef unsigned char byte;

int INVALID_SOCKET=0;

int UdpLog::_id = 0;
int UdpLog::_socket = INVALID_SOCKET;
struct sockaddr * UdpLog::_sockaddr=NULL;
int UdpLog::_srvAddrLen = 0;
int UdpLog::_logId = 0;

int UdpLog::getSingleId(){
	_id++;
	return _id;
}

void UdpLog::sendBlock(const char *block, bool isBegin, int id, int totoallength,int order){
	byte temp[12];
	memset(temp, 0, 12);
	temp[0] = isBegin ? 0 : 1;
	int i = 0;
	int mask = 0xff;
	for (; i < 4; i++)
		temp[i + 1] = (_logId >> (8 * (4 - i - 1))) & mask;
	for (i = 0; i < 4; i++)
		temp[i + 5] = (id >> (8 * (4 - i - 1))) & mask;
	temp[9] = totoallength>>8;
	temp[10] = totoallength & 0xff;
	temp[11] = order;

	char buf[BLOCK_LEN+12];
	memset(buf, '\0', BLOCK_LEN+12);
	memcpy(buf, temp, 12);
	memcpy(buf + 12, block, strlen(block));

	if (_socket==INVALID_SOCKET)
		_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int a=sendto(_socket, buf, sizeof(buf), 0, _sockaddr, _srvAddrLen);
}

void UdpLog::sendMsg(const char *msg){
	int id = getSingleId();
	int size = strlen(msg);
	int start = 0;
	int order = 0;
	while (start < size){
		char buf[BLOCK_LEN];
		memset(buf, '\0', BLOCK_LEN);
		int length = size - start;
		if (length > BLOCK_LEN)
			length = BLOCK_LEN;
		memcpy(buf, msg + start, length);
		bool isBegin = start == 0;
		start += length;
		sendBlock(buf, isBegin, id, size,order);
		order++;
	}
}

void UdpLog::setDst(const char *ip, int port,int logId){
    struct hostent *server = gethostbyname(ip);
    if (server == NULL) {
        return;
    }
	if (_sockaddr)
		delete _sockaddr;
	sockaddr_in *addrServer = new sockaddr_in;
	addrServer->sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&(*addrServer).sin_addr.s_addr, server->h_length);
	addrServer->sin_port = htons(port);
	_srvAddrLen = sizeof(*addrServer);
	_sockaddr = (struct sockaddr*)addrServer;
	_logId = logId;
}

void UdpLog::resetDst(const char *ip, int port, int logId){
	setDst(ip, port, logId);
	char buf[5];
	buf[0] = MASTHEAD_RESETLOGID;
	int mask = 0xff;
	for (int i=0; i < 4; i++)
		buf[i + 1] = _logId & (mask << (8 * (4 - i - 1)));
	if (_socket == INVALID_SOCKET)
		_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int a = sendto(_socket, buf, sizeof(buf), 0, _sockaddr, _srvAddrLen);
}

string getIntStr(int num,int length){
    char numstr[16];
    sprintf(numstr,"%d",num);
    string pre="0000";
    string str=pre+numstr;
    str=str.substr(str.size()-length,length);
    return str;
}

void UdpLog::i(string tag,string msg){
    int tagSize=tag.size();
    int msgSize=msg.size();
    char timechars[16];
    int timenum=time(NULL);
    sprintf(timechars,"%d",timenum);
    string timestr=timechars;
    int timeSize=timestr.size();
    string level="i";
    int levelSize=level.size();

    if(tagSize<=0||tagSize>9999)
        return;
    if(msgSize<=0|| msgSize>9999)
        return;
    if(timeSize<=0|| timeSize>9999)
        return;
    if(levelSize<=0|| levelSize>9999)
        return;

    msg=getIntStr(timeSize,4)+timestr+getIntStr(tagSize,4)+tag+getIntStr(levelSize,4)+level+getIntStr(msgSize,4)+msg;
	sendMsg(msg.c_str());
}

void UdpLog::w(string tag, string msg) {
	int tagSize = tag.size();
	int msgSize = msg.size();
	char timechars[16];
	int timenum = time(NULL);
	sprintf(timechars, "%d", timenum);
	string timestr = timechars;
	int timeSize = timestr.size();
	string level = "w";
	int levelSize = level.size();

	if (tagSize <= 0 || tagSize>9999)
		return;
	if (msgSize <= 0 || msgSize>9999)
		return;
	if (timeSize <= 0 || timeSize>9999)
		return;
	if (levelSize <= 0 || levelSize>9999)
		return;

	msg = getIntStr(timeSize, 4) + timestr + getIntStr(tagSize, 4) + tag + getIntStr(levelSize, 4) + level + getIntStr(msgSize, 4) + msg;
	sendMsg(msg.c_str());
}

void UdpLog::d(string tag, string msg) {
	int tagSize = tag.size();
	int msgSize = msg.size();
	char timechars[16];
	int timenum = time(NULL);
	sprintf(timechars, "%d", timenum);
	string timestr = timechars;
	int timeSize = timestr.size();
	string level = "d";
	int levelSize = level.size();

	if (tagSize <= 0 || tagSize>9999)
		return;
	if (msgSize <= 0 || msgSize>9999)
		return;
	if (timeSize <= 0 || timeSize>9999)
		return;
	if (levelSize <= 0 || levelSize>9999)
		return;

	msg = getIntStr(timeSize, 4) + timestr + getIntStr(tagSize, 4) + tag + getIntStr(levelSize, 4) + level + getIntStr(msgSize, 4) + msg;
	sendMsg(msg.c_str());
}

void UdpLog::e(string tag,string msg){
    int tagSize=tag.size();
    int msgSize=msg.size();
    char timechars[16];
    int timenum=time(NULL);
    sprintf(timechars,"%d",timenum);
    string timestr=timechars;
    int timeSize=timestr.size();
    string level="e";
    int levelSize=level.size();

    if(tagSize<=0||tagSize>9999)
        return;
    if(msgSize<=0|| msgSize>9999)
        return;
    if(timeSize<=0|| timeSize>9999)
        return;
    if(levelSize<=0|| levelSize>9999)
        return;

    msg=getIntStr(timeSize,4)+timestr+getIntStr(tagSize,4)+tag+getIntStr(levelSize,4)+level+getIntStr(msgSize,4)+msg;
    sendMsg(msg.c_str());
}
