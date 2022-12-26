#include "pch.h"
#include "TcpClient.h"
#include "ZXEngine.h"
#include "rtc_base/strings/json.h"

#include <random>
std::default_random_engine engine;
#define random(a,b) (engine()%(b-a)+a)

CTcpClient::CTcpClient()
{
	close_ = false;
	connect_ = false;
	pZXEngine = nullptr;
	tcp_.set_callback(this);

	strSfu = "";
	strMid = "";
	strSdp = "";
	strSid = "";

	nIndex = 0;
	nType = -1;
	nRespOK = -1;
	bRespResult = false;
}

CTcpClient::~CTcpClient()
{
	
}

void CTcpClient::onMessage(const std::string& message)
{
	std::string msg = "websocket recv = " + message;
	ZXEngine::writeLog(msg);

	if (close_)
	{
		return;
	}

	Json::Value jRoot;
	Json::String err;
	Json::CharReaderBuilder build;
	Json::CharReader* pRead = build.newCharReader();
	if (!pRead->parse(message.c_str(), message.c_str() + message.length(), &jRoot, &err))
	{
		ZXEngine::writeLog("recv data parse error");
		return;
	}

	/*
	Json::Value jRoot;
	Json::Reader reader;
	if (!reader.parse(message, jRoot))
	{
		ZXEngine::writeLog("recv data parse error");
		return;
	}*/

	bool bResp = false;
	rtc::GetBoolFromJsonObject(jRoot, "response", &bResp);
	if (bResp)
	{
		int nId = 0;
		if (!rtc::GetIntFromJsonObject(jRoot, "id", &nId))
		{
			ZXEngine::writeLog("recv data id error");
			return;
		}

		if (nId == nIndex)
		{
			bool bOK = false;
			if (!rtc::GetBoolFromJsonObject(jRoot, "ok", &bOK))
			{
				ZXEngine::writeLog("recv data ok error");
				return;
			}

			if (bOK)
			{
				nRespOK = 1;
				if (nType == 10000)
				{
					bRespResult = true;

					Json::Value jData;
					if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}

					Json::Value jUsers;
					if (!rtc::GetValueFromJsonObject(jData, "users", &jUsers))
					{
						ZXEngine::writeLog("recv data users error");
						return;
					}

					Json::Value jPubs;
					if (!rtc::GetValueFromJsonObject(jData, "pubs", &jPubs))
					{
						ZXEngine::writeLog("recv data pubs error");
						return;
					}

					std::vector<Json::Value> jUser;
					if (!rtc::JsonArrayToValueVector(jUsers, &jUser))
					{
						ZXEngine::writeLog("recv data users error");
						return;
					}

					for (size_t i = 0; i < jUser.size(); i++)
					{
						pZXEngine->respPeerJoin(jUser[i]);
					}

					std::vector<Json::Value> jPub;
					if (!rtc::JsonArrayToValueVector(jPubs, &jPub))
					{
						ZXEngine::writeLog("recv data pubs error");
						return;
					}

					for (size_t i = 0; i < jPub.size(); i++)
					{
						pZXEngine->respStreamAdd(jPub[i]);
					}
				}
				if (nType == 10010)
				{
					Json::Value jData;
					if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}

					Json::Value jJsep;
					if (!rtc::GetValueFromJsonObject(jData, "jsep", &jJsep))
					{
						ZXEngine::writeLog("recv data jsep error");
						return;
					}

					std::string sdp;
					if (!rtc::GetStringFromJsonObject(jJsep, "sdp", &sdp))
					{
						ZXEngine::writeLog("recv data sdp error");
						return;
					}
					strSdp = sdp;

					std::string mid;
					if (!rtc::GetStringFromJsonObject(jData, "mid", &mid))
					{
						ZXEngine::writeLog("recv data mid error");
						return;
					}
					strMid = mid;

					std::string sfu;
					if (!rtc::GetStringFromJsonObject(jData, "sfuid", &sfu))
					{
						ZXEngine::writeLog("recv data sfu error");
						return;
					}
					strSfu = sfu;

					bRespResult = true;
				}
				if (nType == 10015)
				{
					Json::Value jData;
					if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}

					Json::Value jJsep;
					if (!rtc::GetValueFromJsonObject(jData, "jsep", &jJsep))
					{
						ZXEngine::writeLog("recv data jsep error");
						return;
					}

					std::string sdp;
					if (!rtc::GetStringFromJsonObject(jJsep, "sdp", &sdp))
					{
						ZXEngine::writeLog("recv data sdp error");
						return;
					}
					strSdp = sdp;

					std::string sid;
					if (!rtc::GetStringFromJsonObject(jData, "sid", &sid))
					{
						ZXEngine::writeLog("recv data sid error");
						return;
					}
					strSid = sid;

					bRespResult = true;
				}
			}
			else
			{
				nRespOK = 0;
			}
		}
	}
	else
	{
		bool bNotification = false;
		if (!rtc::GetBoolFromJsonObject(jRoot, "notification", &bNotification))
		{
			ZXEngine::writeLog("recv data notification error");
			return;
		}

		if (bNotification)
		{
			std::string method;
			if (!rtc::GetStringFromJsonObject(jRoot, "method", &method))
			{
				ZXEngine::writeLog("recv data method error");
				return;
			}
			if (method == "peer-join")
			{
				Json::Value jData;
				if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}

				pZXEngine->respPeerJoin(jData);
			}
			if (method == "peer-leave")
			{
				Json::Value jData;
				if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}

				pZXEngine->respPeerLeave(jData);
			}
			if (method == "stream-add")
			{
				Json::Value jData;
				if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}

				pZXEngine->respStreamAdd(jData);
			}
			if (method == "stream-remove")
			{
				Json::Value jData;
				if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}

				pZXEngine->respStreamRemove(jData);
			}
			if (method == "peer-kick")
			{
				Json::Value jData;
				if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}

				pZXEngine->respPeerKick(jData);
			}
			/*
			if (method == "broadcast")
			{
				Json::Value jData;
				if (!rtc::GetValueFromJsonObject(jRoot, "data", &jData))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}

				Json::Value jData2;
				if (!rtc::GetValueFromJsonObject(jData, "data", &jData2))
				{
					ZXEngine::writeLog("recv data data error");
					return;
				}
				else
				{
					int cmdType;
					if (!rtc::GetIntFromJsonObject(jData2, "cmdType", &cmdType))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}
					std::string strCmdData;;
					if (!rtc::GetStringFromJsonObject(jData2, "cmdData", &strCmdData))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}

					std::string strTargetUid;
					if (!rtc::GetStringFromJsonObject(jData2, "targetUid", &strTargetUid))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}

					std::string senderUid;
					if (!rtc::GetStringFromJsonObject(jData2, "uid", &senderUid))
					{
						ZXEngine::writeLog("recv data data error");
						return;
					}

					pZXEngine->onBroadcastCmd(senderUid.c_str(), strTargetUid.c_str(), cmdType, strCmdData.c_str());
				}
			}*/
		}
	}
}

void CTcpClient::onOpen(const std::string& err)
{
	connect_ = true;
}

void CTcpClient::onClose(const std::string& err)
{
	connect_ = false;
	if (!close_ && pZXEngine != nullptr)
	{
		pZXEngine->respSocketEvent();
	}
}

bool CTcpClient::Start(const char *pServer, int nPort)
{
	// 关闭资源
	Stop();
	// 启动TCP
	close_ = false;
	connect_ = false;
	mutex_.lock();
	bool suc = tcp_.open(pServer, nPort);
	mutex_.unlock();
	return suc;
}

void CTcpClient::Stop()
{
	if (close_)
	{
		return;
	}

	close_ = true;
	connect_ = false;

	mutex_.lock();
	tcp_.close();
	mutex_.unlock();
}

bool CTcpClient::GetConnect()
{
	return connect_;
}

bool CTcpClient::SendJoin()
{
	if (pZXEngine == nullptr)
	{
		return false;
	}
	if (pZXEngine->strRid == "")
	{
		return false;
	}

	int nCount = random(1000000, 9000000);
	nIndex = nCount;

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;
	jsonData["uid"] = pZXEngine->strUid;

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "join";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send join = " + jsonStr;
		ZXEngine::writeLog(msg);

		nRespOK = -1;
		bRespResult = false;
		nType = 10000;
		if (tcp_.send(jsonStr))
		{
			int nCount = 150;
			while (nCount > 0)
			{
				if (nRespOK == 0)
				{
					mutex_.unlock();
					return false;
				}
				if (nRespOK == 1)
				{
					if (bRespResult)
					{
						mutex_.unlock();
						return true;
					}
				}
				if (close_)
				{
					mutex_.unlock();
					return false;
				}
				Sleep(100);
				nCount--;
			}
		}
	}
	mutex_.unlock();
	return false;
}

void CTcpClient::SendLeave()
{
	if (pZXEngine == nullptr)
	{
		return;
	}
	if (pZXEngine->strRid == "")
	{
		return;
	}

	int nCount = random(1000000, 9000000);

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "leave";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send leave = " + jsonStr;
		ZXEngine::writeLog(msg);

		tcp_.send(jsonStr);
	}
	mutex_.unlock();
}

void CTcpClient::SendAlive()
{
	if (pZXEngine == nullptr)
	{
		return;
	}
	if (pZXEngine->strRid == "")
	{
		return;
	}

	int nCount = random(1000000, 9000000);

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "keepalive";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send alive = " + jsonStr;
		ZXEngine::writeLog(msg);

		tcp_.send(jsonStr);
	}
	mutex_.unlock();
}

bool CTcpClient::SendPublish(std::string sdp, bool bAudio, bool bVideo, int audioType, int videoType)
{
	if (pZXEngine == nullptr)
	{
		return false;
	}
	if (pZXEngine->strRid == "")
	{
		return false;
	}

	int nCount = random(1000000, 9000000);
	nIndex = nCount;

	Json::Value jsonJsep;
	jsonJsep["sdp"] = sdp;
	jsonJsep["type"] = "offer";

	Json::Value jsonMinfo;
	jsonMinfo["audio"] = bAudio;
	jsonMinfo["video"] = bVideo;
	jsonMinfo["audiotype"] = audioType;
	jsonMinfo["videotype"] = videoType;

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;
	jsonData["jsep"] = jsonJsep;
	jsonData["minfo"] = jsonMinfo;

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "publish";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send publish = " + jsonStr;
		ZXEngine::writeLog(msg);

		nRespOK = -1;
		bRespResult = false;
		nType = 10010;
		if (tcp_.send(jsonStr))
		{
			int nCount = 150;
			while (nCount > 0)
			{
				if (nRespOK == 0)
				{
					mutex_.unlock();
					return false;
				}
				if (nRespOK == 1)
				{
					if (bRespResult)
					{
						mutex_.unlock();
						return true;
					}
				}
				if (close_)
				{
					mutex_.unlock();
					return false;
				}
				Sleep(100);
				nCount--;
			}
		}
	}
	mutex_.unlock();
	return false;
}

void CTcpClient::SendUnPublish(std::string mid, std::string sfuid)
{
	if (pZXEngine == nullptr)
	{
		return;
	}
	if (pZXEngine->strRid == "")
	{
		return;
	}

	int nCount = random(1000000, 9000000);

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;
	jsonData["mid"] = mid;
	if (sfuid != "")
	{
		jsonData["sfuid"] = sfuid;
	}

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "unpublish";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send unpublish = " + jsonStr;
		ZXEngine::writeLog(msg);

		tcp_.send(jsonStr);
	}
	mutex_.unlock();
}

bool CTcpClient::SendSubscribe(std::string sdp, std::string mid, std::string sfuid)
{
	if (pZXEngine == nullptr)
	{
		return false;
	}
	if (pZXEngine->strRid == "")
	{
		return false;
	}

	int nCount = random(1000000, 9000000);
	nIndex = nCount;

	Json::Value jsonJsep;
	jsonJsep["sdp"] = sdp;
	jsonJsep["type"] = "offer";

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;
	jsonData["jsep"] = jsonJsep;
	jsonData["mid"] = mid;
	if (sfuid != "")
	{
		jsonData["sfuid"] = sfuid;
	}

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "subscribe";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send subscribe = " + jsonStr;
		ZXEngine::writeLog(msg);

		nRespOK = -1;
		bRespResult = false;
		nType = 10015;
		if (tcp_.send(jsonStr))
		{
			int nCount = 150;
			while (nCount > 0)
			{
				if (nRespOK == 0)
				{
					mutex_.unlock();
					return false;
				}
				if (nRespOK == 1)
				{
					if (bRespResult)
					{
						mutex_.unlock();
						return true;
					}
				}
				if (close_)
				{
					mutex_.unlock();
					return false;
				}
				Sleep(100);
				nCount--;
			}
		}
	}
	mutex_.unlock();
	return false;
}

void CTcpClient::SendUnSubscribe(std::string mid, std::string sid, std::string sfuid)
{
	if (pZXEngine == nullptr)
	{
		return;
	}
	if (pZXEngine->strRid == "")
	{
		return;
	}

	int nCount = random(1000000, 9000000);

	Json::Value jsonData;
	jsonData["rid"] = pZXEngine->strRid;
	jsonData["mid"] = mid;
	jsonData["sid"] = sid;
	if (sfuid != "")
	{
		jsonData["sfuid"] = sfuid;
	}

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["id"] = nCount;
	jRoot["method"] = "unsubscribe";
	jRoot["data"] = jsonData;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send unsubscribe = " + jsonStr;
		ZXEngine::writeLog(msg);

		tcp_.send(jsonStr);
	}
	mutex_.unlock();
}

void CTcpClient::sendBroadcastCmd(std::string targetUid, int cmdType, std::string cmdData)
{
	if (pZXEngine == nullptr)
	{
		return;
	}
	if (pZXEngine->strRid == "")
	{
		return;
	}

	int nCount = random(1000000, 9000000);

	Json::Value jsonData0;
	jsonData0["targetUid"] = targetUid;
	jsonData0["cmdType"] = cmdType;
	jsonData0["cmdData"] = cmdData;
	jsonData0["uid"] = pZXEngine->strUid;

	Json::Value jsonData1;
	jsonData1["rid"] = pZXEngine->strRid;
	jsonData1["data"] = jsonData0;

	Json::Value jRoot;
	jRoot["request"] = true;
	jRoot["method"] = "broadcast";
	jRoot["id"] = nCount;
	jRoot["data"] = jsonData1;

	std::string jsonStr;
	//Json::StyledWriter writer;
	//jsonStr = writer.write(jRoot);

	std::ostringstream out;
	Json::StreamWriterBuilder build;
	Json::StreamWriter* pWrite = build.newStreamWriter();
	pWrite->write(jRoot, &out);
	jsonStr = out.str();

	mutex_.lock();
	if (GetConnect())
	{
		std::string msg = "websocket send sendBroadcastCmd = " + jsonStr;
		ZXEngine::writeLog(msg);

		tcp_.send(jsonStr);
	}
	mutex_.unlock();
}
