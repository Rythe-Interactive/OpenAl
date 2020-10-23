#include "audio.hpp"
#include <iostream>
#include <string>
#include <stdio.h>

ALCdevice* Audio::m_device = nullptr;
ALCcontext* Audio::m_context = nullptr;
unsigned char Audio::m_initialized = 0b0;

Audio::Audio(const char* file, ALenum format)
{
	if (!init()) return;
	alGenSources((ALuint)1, &m_source);
	alSourcef(m_source, AL_PITCH, m_pitch);
	//alSourcef(m_source, AL_GAIN, 1);
	alSource3f(m_source, AL_POSITION, 0,0,0);
	alSource3f(m_source, AL_VELOCITY, 0,0,0);
	alSourcei(m_source, AL_LOOPING, AL_FALSE);

	alSourcef(m_source, AL_ROLLOFF_FACTOR, 2.0f);
	alSourcef(m_source, AL_REFERENCE_DISTANCE, 6);
	alSourcef(m_source, AL_MAX_DISTANCE, 15);

	alGenBuffers((ALuint)1, &m_buffer);

	if (mp3dec_load(&m_mp3dec, file, &m_audioInfo, NULL, NULL))
	{
		printf("ERROR: Failed to load Audio file: %s\n", file);
		return;
	}

	alBufferData(m_buffer, format, m_audioInfo.buffer, m_audioInfo.samples * sizeof(mp3d_sample_t), m_audioInfo.hz);
	alSourcei(m_source, AL_BUFFER, m_buffer);

	std::cout << "buffer: \t" << m_audioInfo.buffer <<
		"\nChannels: \t" << m_audioInfo.channels <<
		"\nHz: \t\t" << m_audioInfo.hz <<
		"\nLayer \t\t" << m_audioInfo.layer <<
		"\nSamples: \t" << m_audioInfo.samples <<
		"\navg kbps: \t" << m_audioInfo.avg_bitrate_kbps <<
		"\n-------------------------------------\n";
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

float Audio::getPitch()
{
	return m_pitch;
}

void Audio::gain(float gain)
{
	alSourcef(m_source, AL_GAIN, gain);
}

void Audio::setFormat(ALenum format)
{
	alBufferData(m_buffer, format, m_audioInfo.buffer, m_audioInfo.samples * sizeof(mp3d_sample_t), m_audioInfo.hz);
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

void Audio::query_information()
{
	const ALchar* vendor = alGetString(AL_VENDOR);
	const ALchar* version = alGetString(AL_VERSION);
	const ALchar* renderer = alGetString(AL_RENDERER);
	const ALchar* openALExtensions = alGetString(AL_EXTENSIONS);
	const ALchar* ALCExtensions = alcGetString(m_device, ALC_EXTENSIONS);
	//const ALchar* auxSends = alGetString(AL_MAX);
	printf("\tOpenAL info:\n\tVendor: %s\n\tVersion: %s\n\tRenderer: %s\n\tOpenAl Extensions: %s\n\tALC Extensions: %s\n",
		vendor, version, renderer, openALExtensions, ALCExtensions);
	ALCint srate;
	alcGetIntegerv(m_device, ALC_FREQUENCY, 1, &srate);
	std::cout << "OpenAl device freq: " << srate << std::endl;
}

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

		ALCint srate;
		alcGetIntegerv(m_device, ALC_FREQUENCY, 1, &srate);
		std::cout << "OpenAl device freq: " << srate << std::endl;
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
		query_information();
		const char* hrtf = alcGetStringiSOFT(m_device, ALC_SOFT_HRTF, 0);
		printf("\nHRTF: %s\n", hrtf);
	}
	if (!(m_initialized & 16)) listenerPosition(0, 0, 0);
	if (!(m_initialized & 32)) listenerVelocity(0, 0, 0);
	if (!(m_initialized & 64)) listenerOrientation(0, 0, 1.0f, 0, 1.0f, 0);

	return true;
}