#pragma once

// video frame callback
// video_type = 0 camera, 1 screen, 2 file
// video_frame type = i420
typedef void(*video_frame_callback)(const char* uid, int video_type, const unsigned char* data_y, const unsigned char* data_u, const unsigned char* data_v,
	int stride_y, int stride_u, int stride_v, unsigned int width, unsigned int height);

// audio frame callback
// audio_type = 0 mic, 1 other, 2 file
typedef void (*audio_frame_callback)(const char* uid, int audio_type, const void* audio_data, 
	int bits_per_sample, int sample_rate, size_t number_of_channels, size_t number_of_frames, double audioLevel);

// log callback
typedef void(*log_callback)(const char* msg);
