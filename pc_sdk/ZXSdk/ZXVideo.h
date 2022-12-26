#pragma once
#include "api/media_stream_interface.h"

class ZXPeerScreen;
class ZXPeerRemote;
class ZXVideoObserver : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
	ZXVideoObserver();
	virtual ~ZXVideoObserver();

	// VideoSinkInterface implementation
	void OnFrame(const webrtc::VideoFrame& frame) override;

public:
	ZXPeerScreen *pZXPeerScreen;
	ZXPeerRemote *pZXPeerRemote;
};

