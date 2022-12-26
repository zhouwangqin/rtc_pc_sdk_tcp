#include "pch.h"
#include "ZXBase.h"

// rtc
const char kStreamId[] = "ARDAMS";
const char kAudioLabel[] = "ARDAMSa0";
const char kVideoLabel[] = "ARDAMSv0";

// video
const std::string VIDEO_VP8_INTEL_HW_ENCODER_FIELDTRIAL = "WebRTC-IntelVP8/Enabled/";
const std::string VIDEO_FLEXFEC_FIELDTRIAL = "WebRTC-FlexFEC-03-Advertised/Enabled/WebRTC-FlexFEC-03/Enabled/";

// biz server
uint16_t g_server_port = 8443;
std::string g_server_ip = "49.235.93.74";
// turn server
std::string g_relay_server_ip = "121.4.240.130:3478";

// msg
const int socket_disconnet_ = 1000;
const int set_offer_sdp_ok = 10000;

// log callback
log_callback log_callback_ = nullptr;