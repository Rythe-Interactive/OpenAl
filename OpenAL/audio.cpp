#include "audio.hpp"
#include <iostream>
#include <string>

ALCdevice* Audio::m_device = nullptr;
ALCcontext* Audio::m_context = nullptr;
unsigned char Audio::m_initialized = 0b0;

Audio::Audio(const char* file)
{
	if (!init()) return;
	alGenSources((ALuint)1, &m_source);
	alSourcef(m_source, AL_PITCH, m_pitch);
	alSourcef(m_source, AL_GAIN, 1);
	alSource3f(m_source, AL_POSITION, 0,0,0);
	alSource3f(m_source, AL_VELOCITY, 0,0,0);
	alSourcei(m_source, AL_LOOPING, AL_FALSE);

	alGenBuffers((ALuint)1, &m_buffer);

	if (mp3dec_load(&m_mp3dec, file, &m_audioInfo, NULL, NULL))
	{
		printf("ERROR: Failed to load Audio file: %s\n", file);
		return;
	}

	alBufferData(m_buffer, AL_FORMAT_STEREO16, m_audioInfo.buffer, m_audioInfo.samples * sizeof(mp3d_sample_t), m_audioInfo.hz);
	alSourcei(m_source, AL_BUFFER, m_buffer);
}

Audio::~Audio()
{
	free(m_audioInfo.buffer);
	alDeleteSources(1, &m_source);
	alDeleteBuffers(1, &m_buffer);
}

void Audio::play()
{
	alSourcePlay(m_source);
}

bool Audio::isPlaying()
{
	ALint sourceState;
	alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
	return sourceState == AL_PLAYING;
}

void Audio::pause()
{
	alSourcePause(m_source);
}

void Audio::stop()
{
	alSourceStop(m_source);
}

void Audio::setPitch(float pitch)
{
	m_pitch = pitch;
	alSourcef(m_source, AL_PITCH, pitch);
}

void Audio::gain(float gain)
{
	alSourcef(m_source, AL_GAIN, gain);
}

void Audio::openal_error()
{
	ALCenum error;
	error = alGetError();
	if (error != AL_NO_ERROR)
		std::cout << "ERROR: OpenAl error: " << error << std::endl;
}

#pragma region listener

void Audio::listenerPosition(const float x, const float y, const float z)
{
	m_initialized |= 16;
	alListener3f(AL_POSITION, x, y, z);
}

void Audio::listenerVelocity(const float x, const float y, const float z)
{
	m_initialized |= 32;
	alListener3f(AL_VELOCITY, x, y, z);
}

void Audio::listenerOrientation(const float x0, const float y0, const float z0, const float x1, const float y1, const float z1)
{
	float orientation[] = { x0, y0, z0, x1, y1, z1 };
	Audio::listenerOrientation(orientation);
}

void Audio::listenerOrientation(const float* values)
{
	m_initialized |= 64;
	alListenerfv(AL_ORIENTATION, values);
}

#pragma endregion

bool Audio::init()
{
	if (!(m_initialized & 1) || (m_initialized & 2))
	{
		//not initialized
		m_initialized |= 1; //set bit 0
		m_initialized &= ~2; //clear bit 1
		m_device = alcOpenDevice(NULL);
		if (!m_device)
		{
			std::cout << "ERROR: ALC Device Error" << std::endl;
			m_initialized |= 2; // set bit 1
			return false;
		}
	}
	if (!(m_initialized & 4))
	{
		//set bit 2
		m_initialized |= 4; //set bit 2
		m_initialized &= ~8; // clear bit 4
		m_context = alcCreateContext(m_device, NULL);
		if (!alcMakeContextCurrent(m_context))
		{
			std::cout << "ERROR: Failed to create OpenAl context\n";
			openal_error();
			m_initialized |= 8; // set bit 4
			return false;
		}
	}
	if (!(m_initialized & 16)) listenerPosition(0, 0, 1.0f);
	if (!(m_initialized & 32)) listenerVelocity(0, 0, 0);
	if (!(m_initialized & 64)) listenerOrientation(0, 0, 1.0f, 0, 1.0f, 0);
	return true;
}