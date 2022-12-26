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

	// ������־�ص�
	WEBRTC_API void setLogDebug(log_callback callback);

	// ����Զ����Ƶ�ص�,���ڿռ���Ч����
	WEBRTC_API void setRemoteAudio(audio_frame_callback callback);

	// ���ñ�����Ƶ���ص�,���ڱ�����Ⱦ����
	WEBRTC_API void setLocalVideo(video_frame_callback callback);
	
	// ����Զ����Ƶ���ص�,����Զ����Ⱦ����
	WEBRTC_API void setRemoteVideo(video_frame_callback callback);

	// ���÷�������ַ
	WEBRTC_API void setServerIp(char* ip, unsigned short port);

	// ����sdk
	WEBRTC_API bool initSdk(char *uid);

	// �ͷ�sdk
	WEBRTC_API void freeSdk();

	// ���뷿��
	WEBRTC_API bool joinRoom(char *rid);

	// �뿪����
	WEBRTC_API void leaveRoom();

	// ������Ƶ��������
	// true-����,false-�ر�,Ĭ�Ͽ���
	WEBRTC_API void setPublish(bool bPub);

	// ������Ļ�����ܿ���
	// true-����,false-�ر�,Ĭ�Ϲر�
	WEBRTC_API void setScreen(bool bPub);
	// ������Ļ������������
	// true-����,false-�ر�,Ĭ�Ͽ���
	WEBRTC_API void setScreenPub(bool bPub);
	// ������Ļ����֡��(5-15)
	WEBRTC_API void setFrameRate(int nFrameRate);

#ifdef __cplusplus
}
#endif