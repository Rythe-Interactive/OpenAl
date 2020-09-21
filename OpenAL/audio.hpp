#include <AL/al.h>
#include <AL/alc.h>

#include "minimp3.h"
#include "minimp3_ex.h"

#ifndef AUDIO_HPP
#define AUDIO_HPP

class Audio
{
public:
	static void listenerPosition(const float x, const float y, const float z);
	static void listenerVelocity(const float x, const float y, const float z);
	static void listenerOrientation(const float x0, const float y0, const float z0, const float x1, const float y1, const float z1);
	static void listenerOrientation(const float* values);

	Audio(const char* file);
	~Audio();
	void play();

private:
	static ALCdevice* m_device;
	static ALCcontext* m_context;
	// uint for storing initialized flags
	// 0 - device init, 1 - device error
	// 2 - context init, 3 - context error
	// 4 - listener position, 5 - listener velocity
	// 6 - listener orientation
	static unsigned char m_initialized;
	// initializes device and context
	// might initialize listener position, velocity
	// and orientation if not setup
	static bool init();
	static void openal_error();

	ALuint m_source;
	ALuint m_buffer;
	mp3dec_t m_mp3dec;
	mp3dec_file_info_t m_audioInfo;

};

#endif // !AUDIO_HPP
