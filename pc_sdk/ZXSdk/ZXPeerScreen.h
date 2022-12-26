#pragma once
#include "ZXVideo.h"
#include "ZXScreen.h"
#include "ZXSdpListen.h"
#include "api/peer_connection_interface.h"

class ZXEngine;
class ZXPeerScreen : public webrtc::PeerConnectionObserver,
					 public rtc::MessageHandler
{
public:
	ZXPeerScreen();
	virtual ~ZXPeerScreen();

	// ��������
	void StartPublish();
	// ȡ������
	void StopPublish();

	// ����Offer
	void CreateOffer();
	// offer�����ɹ�
	void CreateSdpSuc(webrtc::SessionDescriptionInterface* sdp);
	// offer����ʧ��
	void CreateSdpFail(std::string error);

	// ���ñ���sdp
	void SetLocalDescription(webrtc::SessionDescriptionInterface* sdp);
	// ���ñ���sdp�ɹ�
	void OnSetLocalSdpSuc();
	// ���ñ���sdpʧ��
	void OnSetLoaclSdpFail(std::string error);

	// ����Զ��sdp
	void SetRemoteDescription(webrtc::SessionDescriptionInterface* sdp);
	// ����Զ��sdp�ɹ�
	void OnSetRemoteSdpSuc();
	// ����Զ��sdpʧ��
	void OnSetRemoteSdpFail(std::string error);

	// ��������
	void SendPublish(std::string sdp);
	// ����ȡ������
	void SendUnPublish();

private:
	// PeerConnection����
	bool InitPeerConnection();
	void FreePeerConnection();

	// ���÷��Ͷ�����
	void SetBitrate();

	//
	// PeerConnectionObserver implementation
	//
	void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {}
	void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
	void OnRenegotiationNeeded() override {}
	void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
	void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
	void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override {}

	// MessageHandler implementation
	void OnMessage(rtc::Message* msg) override;

public:
	// ����
	std::string strUid;
	std::string strMid;
	std::string strSfu;

	// ����״̬
	int nLive;
	// �˳����
	bool bClose;
	// �ϲ����
	ZXEngine* pZXEngine;
	// ��Ƶ�ص�����
	ZXVideoObserver local_video_observer_;

	// �ɼ�֡��
	int nCapFrameRate;
	// �������kbps
	int nMinBitRate;
	// �������kbps
	int nMaxBitRate;
	// ��ʼ����kbps
	int nStartBitRate;

private:
	// offer sdp
	std::string sdp_;
	// rtc ����
	rtc::scoped_refptr<ZXScreen> video_device_;
	rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;

	// sdp �ص�
	rtc::scoped_refptr<OfferCreateSessionDescriptionObserver> pOfferCreateSdpObserver;
	rtc::scoped_refptr<OfferSetSessionDescriptionObserver> pOfferSetSdpObserver;
	rtc::scoped_refptr<AnswerSetSessionDescriptionObserver> pAnswerSetSdpObserver;
};

