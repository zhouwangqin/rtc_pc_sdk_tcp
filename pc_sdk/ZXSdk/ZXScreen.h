#pragma once
#include <mutex>
#include "api/video/i420_buffer.h"
#include "api/peer_connection_interface.h"
#include "media/base/adapted_video_track_source.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"

class ZXPeerScreen;
class ZXScreen : public rtc::AdaptedVideoTrackSource,
				 public webrtc::DesktopCapturer::Callback,
				 public rtc::MessageHandler
{
public:
	ZXScreen();
	virtual ~ZXScreen();

	// �ɼ�����
	bool initCapturer();
	void freeCapturer();
	bool startCapturer();
	void stopCapture();

private:
	// MessageHandler implementation
	void OnMessage(rtc::Message* msg) override;

	// AdaptedVideoTrackSource implementation
	bool is_screencast() const override;
	absl::optional<bool> needs_denoising() const override;
	webrtc::MediaSourceInterface::SourceState state() const override;
	bool remote() const override;

	// DesktopCapturer::Callback implementation
	void OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame) override;

	// �ɼ�һ֡
	void CaptureFrame();

public:
	ZXPeerScreen *pZXPeerScreen;

private:
	bool open_;
	std::mutex mutex_;
	// �ɼ�����
	rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
	std::unique_ptr<webrtc::DesktopAndCursorComposer> capturer_;
};

