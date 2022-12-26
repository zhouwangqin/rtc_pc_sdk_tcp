#pragma once
#include "ZXSdpListen.h"
#include "api/peer_connection_interface.h"

class ZXEngine;
class ZXPeerLocal : public webrtc::PeerConnectionObserver,
	public webrtc::AudioTrackSinkInterface,
	public rtc::MessageHandler
{
public:
	ZXPeerLocal();
	virtual ~ZXPeerLocal();

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

	// AudioTrackSinkInterface implementation
	void OnData(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels, size_t number_of_frames) override;

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

private:
	// offer sdp
	std::string sdp_;
	// rtc ����
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;

	// sdp �ص�
	rtc::scoped_refptr<OfferCreateSessionDescriptionObserver> pOfferCreateSdpObserver;
	rtc::scoped_refptr<OfferSetSessionDescriptionObserver> pOfferSetSdpObserver;
	rtc::scoped_refptr<AnswerSetSessionDescriptionObserver> pAnswerSetSdpObserver;
};