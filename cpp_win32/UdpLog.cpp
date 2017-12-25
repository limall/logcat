#include "UdpLog.h"
#include "singleJson.hpp"
#include "json.h"

std::mutex * UdpLog::_mt=new std::mutex;
int UdpLog::_id = 0;
SOCKET UdpLog::_socket = INVALID_SOCKET;
struct sockaddr * UdpLog::_sockaddr=NULL;
int UdpLog::_srvAddrLen = 0;
int UdpLog::_logId = 0;

int UdpLog::getSingleId(){
	int toReturn;
	_mt->lock();
	toReturn = _id;
	_id++;
	_mt->unlock();
	return toReturn;
}

std::string stackTrace(){
	std::ostringstream stack_info_str_stream_;
#ifdef _WIN32
	UINT max_name_length_;              // Max length of symbols' name.
	CONTEXT context_;                   // Store register addresses.
	STACKFRAME64 stackframe_;           // Call stack.
	HANDLE process_, thread_;           // Handle to current process & thread.
	PSYMBOL_INFO symbol_;               // Debugging symbol's information.
	IMAGEHLP_LINE64 source_info_;       // Source information (file name & line number)
	DWORD displacement_;                // Source line displacement.
	enum { MAX_NAME_LENGTH = 256 };  // max length of symbols' name.
	// Initialize PSYMBOL_INFO structure.  
	// Allocate a properly-sized block.  
	symbol_ = (PSYMBOL_INFO)malloc(sizeof(SYMBOL_INFO)+(MAX_NAME_LENGTH - 1) * sizeof(TCHAR));
	memset(symbol_, 0, sizeof(SYMBOL_INFO)+(MAX_NAME_LENGTH - 1) * sizeof(TCHAR));
	symbol_->SizeOfStruct = sizeof(SYMBOL_INFO);  // SizeOfStruct *MUST BE* set to sizeof(SYMBOL_INFO).  
	symbol_->MaxNameLen = MAX_NAME_LENGTH;
	// Initialize IMAGEHLP_LINE64 structure.  
	memset(&source_info_, 0, sizeof(IMAGEHLP_LINE64));
	source_info_.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	// Initialize STACKFRAME64 structure.  
	RtlCaptureContext(&context_);  // Get context.  
	memset(&stackframe_, 0, sizeof(STACKFRAME64));
	stackframe_.AddrPC.Offset = context_.Eip;  // Fill in register addresses (EIP, ESP, EBP).  
	stackframe_.AddrPC.Mode = AddrModeFlat;
	stackframe_.AddrStack.Offset = context_.Esp;
	stackframe_.AddrStack.Mode = AddrModeFlat;
	stackframe_.AddrFrame.Offset = context_.Ebp;
	stackframe_.AddrFrame.Mode = AddrModeFlat;
	stack_info_str_stream_.str("");
	process_ = GetCurrentProcess();  // Get current process & thread.  
	thread_ = GetCurrentThread();
	// Initialize dbghelp library.  
	if (!SymInitialize(process_, NULL, TRUE)) {
		stack_info_str_stream_ << "<br>Initialize dbghelp library ERROR!</br>";
	}
	stack_info_str_stream_ << "<br>Call stack:</br>";
	// Enumerate call stack frame.  
	bool isFirst = true;
	int num = 0;
	while (StackWalk64(IMAGE_FILE_MACHINE_I386, process_, thread_, &stackframe_,
		&context_, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
		if (stackframe_.AddrFrame.Offset == 0) {  // End reaches.  
			break;
		}
		if (num > 10){
			stack_info_str_stream_ << "<br>...</br>";
			break;
		}
		if (isFirst){
			isFirst = false;
			continue;
		}
		if (SymFromAddr(process_, stackframe_.AddrPC.Offset, NULL, symbol_)) {  // Get symbol.  
			stack_info_str_stream_ << "<br>==>" << symbol_->Name << "</br>";
			num++;
		}
		if (SymGetLineFromAddr64(process_, stackframe_.AddrPC.Offset, &displacement_, &source_info_)) {
			// Get source information.  
			stack_info_str_stream_ << " [" << source_info_.FileName << ":" << source_info_.LineNumber << "]";
		}
		else {
			if (GetLastError() == 0x1E7) {  // If err_code == 0x1e7, no symbol was found.  
				stack_info_str_stream_ << "No debug symbol loaded for this function.";
			}
		}
	}
#endif  // _WIN32
	return stack_info_str_stream_.str();
}

//ǰ8λΪ�Ƿ��ǿ�ͷ������������32λlogid,��������32λid����������16λ�ܳ��ȣ���������8λorder����������128byte�����ݡ�
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
	if (_sockaddr)
		delete _sockaddr;
	sockaddr_in *addrServer = new sockaddr_in;
	addrServer->sin_family = AF_INET;
	addrServer->sin_addr.s_addr = inet_addr(ip);
	addrServer->sin_port = htons(port);
	_srvAddrLen = sizeof(*addrServer);
	_sockaddr = (struct sockaddr*)addrServer;

	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

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

void UdpLog::i(const char *tag, const char *msg){
	SingleJson sj;
	if (tag == NULL){
		char buf[4];
		sprintf(buf, "%d", _logId);
		sj._strings["tag"] = buf;
	}else
		sj._strings["tag"] = tag;
	sj._ints["time"] = time(NULL);
	sj._strings["level"] = "i";
	sj._strings["msg"] = msg;
	std::string str = sj.stringify();
	sendMsg(str.c_str());
}

void UdpLog::d(const char *tag, const char *msg){
	SingleJson sj;
	if (tag == NULL){
		char buf[4];
		sprintf(buf, "%d", _logId);
		sj._strings["tag"] = buf;
	}
	else
		sj._strings["tag"] = tag;
	sj._ints["time"] = time(NULL);
	sj._strings["level"] = "d";
	sj._strings["msg"] = msg;
	std::string str = sj.stringify();
	sendMsg(str.c_str());
}

void UdpLog::e(const char *tag, const char *msg){
	SingleJson sj;
	if (tag == NULL){
		char buf[4];
		sprintf(buf, "%d", _logId);
		sj._strings["tag"] = buf;
	}
	else
		sj._strings["tag"] = tag;
	sj._ints["time"] = time(NULL);
	sj._strings["level"] = "e";
	sj._strings["msg"] = msg;
	std::string str = sj.stringify();
	sendMsg(str.c_str());
}

void UdpLog::w(const char *tag, const char *msg) {
	SingleJson sj;
	if (tag == NULL){
		char buf[4];
		sprintf(buf, "%d", _logId);
		sj._strings["tag"] = buf;
	}
	else
		sj._strings["tag"] = tag;
	sj._ints["time"] = time(NULL);
	sj._strings["level"] = "w";
	sj._strings["msg"] = msg;
	std::string str = sj.stringify();
	sendMsg(str.c_str());
}

std::string UdpLog::format(char *fmt, va_list ap){
	std::string str = "";
	char buf[32];
	char *p;
	char *sval;
	double dval;
	int ival;
	int xval;
	for (p = fmt; *p; p++)
	{
		if (*p != '%')
		{
			str += *p;
			continue;
		}
		switch (*++p)
		{
		case 'd':
			ival = va_arg(ap, int);
			sprintf(buf, "%d", ival);
			str += buf;
			break;
		case 'f':
			dval = va_arg(ap, double);
			sprintf(buf, "%f", dval);
			str += buf;
			break;
		case 's':
			for (sval = va_arg(ap, char *); *sval; sval++)
			{
				str += *sval;
			}
			break;
		case 'x':
			xval = va_arg(ap, int);
			sprintf(buf, "%x", xval);
			str += buf;
			break;
		default:
			str += *p;
			break;
		}
	}
	va_end(ap);
	return str;
}

void UdpLog::si(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);
	char logid[4];
	sprintf(logid, "%d", _logId);
	i(logid, str.c_str());
}

void UdpLog::sd(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	char logid[4];
	sprintf(logid, "%d", _logId);
	d(logid, str.c_str());
}

void UdpLog::sw(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	char logid[4];
	sprintf(logid, "%d", _logId);
	w(logid, str.c_str());
}

void UdpLog::se(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	char logid[4];
	sprintf(logid, "%d", _logId);
	e(logid, str.c_str());
}

void UdpLog::iBytes(const char *tag, unsigned char *bytes, int len){
	std::string temp = "";
	for (int i = 0; i < len; i++){
		char num[8];
		if (i == len - 1)
			sprintf(num, "%x", *(bytes + i));
		else
			sprintf(num, "%x,", *(bytes + i));
		temp += num;
	}

	char head[16];
	sprintf(head, "%d ", len);
	temp = head + temp;
	i(tag, temp.c_str());
}

void UdpLog::iStackTrace(char *tag, ...){
	va_list ap;
	va_start(ap, tag);
	std::string tagstr = format(tag, ap);
	std::string msg = stackTrace();
	i(tagstr.c_str(), msg.c_str());
}

void UdpLog::dStackTrace(char *tag, ...){
	va_list ap;
	va_start(ap, tag);
	std::string tagstr = format(tag, ap);
	std::string msg = stackTrace();
	d(tagstr.c_str(), msg.c_str());
}

void UdpLog::wStackTrace(char *tag, ...){
	va_list ap;
	va_start(ap, tag);
	std::string tagstr = format(tag, ap);
	std::string msg = stackTrace();
	w(tagstr.c_str(), msg.c_str());
}

void UdpLog::eStackTrace(char *tag, ...){
	va_list ap;
	va_start(ap, tag);
	std::string tagstr = format(tag, ap);
	std::string msg = stackTrace();
	e(tagstr.c_str(), msg.c_str());
}

UdpLog *UdpLog::Tag(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	UdpLog *udpLog = new UdpLog();
	udpLog->_tag = str;
	return udpLog;
}

void UdpLog::logi(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	i(_tag.c_str(),str.c_str());
	delete this;
}

void UdpLog::logd(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	d(_tag.c_str(), str.c_str());
	delete this;
}

void UdpLog::logw(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	w(_tag.c_str(), str.c_str());
	delete this;
}

void UdpLog::loge(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	std::string str = format(fmt, ap);

	e(_tag.c_str(), str.c_str());
	delete this;
}
