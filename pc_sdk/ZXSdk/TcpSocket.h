#pragma once
#include <string>
#include <thread>
#include <WinSock2.h>

class CTcpCall
{
public:
	virtual ~CTcpCall() {};
	virtual void onMessage(const std::string& msg) = 0;
	virtual void onClose(const std::string& err) {};
	virtual void onOpen(const std::string& err) {};
};

class CTcpSocket
{
public:
	CTcpSocket();
	virtual ~CTcpSocket();

public:
	void set_callback(CTcpCall* call);
	bool open(const std::string ip, int port);
	bool send(const std::string& msg);
	void close();

private:
	static void work_thread(CTcpSocket* socket);

private:
	bool m_bInit;
	SOCKET m_socket;
	CTcpCall* m_pCall;
	CRITICAL_SECTION m_csLock;
};

