#include "pch.h"
#include "TcpSocket.h"

CTcpSocket::CTcpSocket()
{
	m_bInit = false;
	m_pCall = nullptr;
	m_socket = INVALID_SOCKET;
	InitializeCriticalSection(&m_csLock);
}

CTcpSocket::~CTcpSocket()
{
	DeleteCriticalSection(&m_csLock);
}

void CTcpSocket::set_callback(CTcpCall* call)
{
	m_pCall = call;
}

bool CTcpSocket::open(const std::string ip, int port)
{
	EnterCriticalSection(&m_csLock);
	WSADATA wsaData = { 0 };
	WORD wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		LeaveCriticalSection(&m_csLock);
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 && HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return false;
	}

	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return false;
	}
	
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	if (::connect(m_socket, (sockaddr*)&servAddr, sizeof(sockaddr)) != 0)
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return false;
	}
	
	m_bInit = true;
	std::thread thread(CTcpSocket::work_thread, this);
	thread.detach();
	LeaveCriticalSection(&m_csLock);

	if (m_pCall != nullptr)
	{
		m_pCall->onOpen("tcp connect is ok");
	}
	return true;
}

bool CTcpSocket::send(const std::string& msg)
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		int nSize = msg.length();
		char* pMsg = new char[nSize + 2];
		pMsg[0] = nSize & 0xFF;
		pMsg[1] = ((nSize>>8) & 0xFF);
		memcpy(pMsg + 2, msg.c_str(), nSize);

		int nIndex = nSize + 2;
		while (nIndex > 0)
		{
			int nSend = ::send(m_socket, pMsg + nSize + 2 - nIndex, nIndex, 0);
			if (nSend >= 0)
			{
				nIndex -= nSend;
			}
			else
			{
				delete[]pMsg;
				LeaveCriticalSection(&m_csLock);
				return false;
			}
		}

		delete []pMsg;
		LeaveCriticalSection(&m_csLock);
		return true;
	}
	LeaveCriticalSection(&m_csLock);
	return false;
}

void CTcpSocket::close()
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		m_bInit = false;
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		WSACleanup();
	}
	LeaveCriticalSection(&m_csLock);
}

void CTcpSocket::work_thread(CTcpSocket* socket)
{
	while (true)
	{
		if (socket->m_bInit)
		{
			unsigned char szLen[2] = { 0 };
			int nRecv = ::recv(socket->m_socket, (char*)szLen, 2, 0);
			if (nRecv <= 0)
			{
				if (socket->m_pCall != nullptr)
				{
					socket->m_pCall->onClose("tcp read len error");
				}
				return;
			}

			int nLen = szLen[0] + (szLen[1] * 256);
			if (nLen > 4096)
			{
				if (socket->m_pCall != nullptr)
				{
					socket->m_pCall->onClose("tcp read len error");
				}
				return;
			}

			char* pszData = new char[nLen];
			nRecv = ::recv(socket->m_socket, (char*)pszData, nLen, 0);
			if (nRecv <= 0)
			{
				if (socket->m_pCall != nullptr)
				{
					socket->m_pCall->onClose("tcp read data error");
				}
				return;
			}

			std::string msg = pszData;
			if (socket->m_pCall != nullptr)
			{
				socket->m_pCall->onMessage(msg);
			}

			delete []pszData;
		}
	}
}
