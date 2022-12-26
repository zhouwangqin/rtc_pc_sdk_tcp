#pragma once
#include "ZXListen.h"

#ifdef WEBRTC_EXPORTS
#define WEBRTC_API __declspec(dllexport)
#else
#define WEBRTC_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	// 设置日志回调
	WEBRTC_API void setLogDebug(log_callback callback);

	// 设置远端音频回调,用于空间音效处理
	WEBRTC_API void setRemoteAudio(audio_frame_callback callback);

	// 设置本地视频流回调,用于本地渲染处理
	WEBRTC_API void setLocalVideo(video_frame_callback callback);
	
	// 设置远端视频流回调,用于远端渲染处理
	WEBRTC_API void setRemoteVideo(video_frame_callback callback);

	// 设置服务器地址
	WEBRTC_API void setServerIp(char* ip, unsigned short port);

	// 加载sdk
	WEBRTC_API bool initSdk(char *uid);

	// 释放sdk
	WEBRTC_API void freeSdk();

	// 加入房间
	WEBRTC_API bool joinRoom(char *rid);

	// 离开房间
	WEBRTC_API void leaveRoom();

	// 设置音频推流开关
	// true-开启,false-关闭,默认开启
	WEBRTC_API void setPublish(bool bPub);

	// 设置屏幕共享功能开关
	// true-开启,false-关闭,默认关闭
	WEBRTC_API void setScreen(bool bPub);
	// 设置屏幕共享推流开关
	// true-开启,false-关闭,默认开启
	WEBRTC_API void setScreenPub(bool bPub);
	// 设置屏幕共享帧率(5-15)
	WEBRTC_API void setFrameRate(int nFrameRate);

#ifdef __cplusplus
}
#endif