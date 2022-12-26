#pragma once
#include <mutex>
#include "TcpSocket.h"

class ZXEngine;
class CTcpClient : public CTcpCall
{
public:
	CTcpClient();
	virtual ~CTcpClient();

public:
	void onMessage(const std::string& message) override;
	void onOpen(const std::string& err) override;
	void onClose(const std::string& err) override;

	// 启动
	bool Start(const char *pServer, int nPort);

	// 停止
	void Stop();

	// 返回连接状态
	bool GetConnect();

	/*
	  "request":true
	  "id":3764139
	  "method":"join"
	  "data":{
		"rid":"room"
		"uid":"111"
	  }
	*/
	// 加入房间
	bool SendJoin();

	/*
	  "request":true
	  "id":3764139
	  "method":"leave"
	  "data":{
		"rid":"room"
	  }
	*/
	// 退出房间
	void SendLeave();

	/*
	 "request":true
	 "id":3764139
	 "method":"keepalive"
	 "data":{
	   "rid":"room"
	 }
	*/
	// 发送心跳
	void SendAlive();

	/*
	  "request":true
	  "id":3764139
	  "method":"publish"
	  "data":{
		"rid":"room",
		"jsep":{"type":"offer","sdp":"..."},
		"minfo":{
			"audio":true,
			"video":true,
			"audiotype":0,
			"videotype":0,
		}
	  }
	*/
	// 发布流
	bool SendPublish(std::string sdp, bool bAudio, bool bVideo, int audioType, int videoType);

	/*
	  "request":true
	  "id":3764139
	  "method":"unpublish"
	  "data":{
		"rid":"room",
		"mid":"64236c21-21e8-4a3d-9f80-c767d1e1d67f#ABCDEF",
		"sfuid":"shenzhen-sfu-1", (可选)
	  }
	*/
	// 取消发布流
	void SendUnPublish(std::string mid, std::string sfuid);

	/*
	  "request":true
	  "id":3764139
	  "method":"subscribe"
	  "data":{
		"rid":"room",
		"mid":"64236c21-21e8-4a3d-9f80-c767d1e1d67f#ABCDEF",
		"jsep":{"type":"offer","sdp":"..."},
		"sfuid":"shenzhen-sfu-1", (可选)
	  }
	*/
	// 订阅流
	bool SendSubscribe(std::string sdp, std::string mid, std::string sfuid);

	/*
	  "request":true
	  "id":3764139
	  "method":"unsubscribe"
	  "data":{
		"rid": "room",
		"mid": "64236c21-21e8-4a3d-9f80-c767d1e1d67f#ABCDEF"
		"sid": "64236c21-21e8-4a3d-9f80-c767d1e1d67f#ABCDEF"
		"sfuid":"shenzhen-sfu-1", (可选)
	  }
	*/
	// 取消订阅流
	void SendUnSubscribe(std::string mid, std::string sid, std::string sfuid);

	/*
		"request":true
		"id":3764139
		"method":"broadcast"
		"data":{
			"rid": "room",
			"data": "$date"
		}
	*/
	void sendBroadcastCmd(std::string targetUid, int cmdType, std::string cmdData);

public:
	// 上层对象
	ZXEngine* pZXEngine;
	// TCP连接对象
	CTcpSocket tcp_;

	// 临时变量
	std::string strSfu;
	std::string strMid;
	std::string strSdp;
	std::string strSid;

	bool close_;
	bool connect_;
	std::mutex mutex_;

	// 信令参数
	int nIndex;
	int nType;
	int nRespOK;
	bool bRespResult;
};

