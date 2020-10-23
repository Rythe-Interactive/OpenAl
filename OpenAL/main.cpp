#include <iostream>
#include <string>
#include <AL/al.h>
#include <AL/alc.h>
#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>
#include "wav_loader.hpp"

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"

#include <fstream>
#include <iostream>

void openal_info(ALCdevice* dev);
void setListener();
void openal_error();

int main()
{
	ALCdevice* device = alcOpenDevice(NULL);
	if (!device) std::cout << "Device failed to init" << std::endl;
	ALCcontext* context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) std::cout << "Context failed to be current" << std::endl;
	
	openal_error();
	setListener();

	ALuint source;
	ALuint buffer;

	alGenSources(1, &source);
	openal_error();
	alSourcef(source, AL_PITCH, 1);
	openal_error();
	alSourcef(source, AL_GAIN, 1);
	openal_error();
	alSource3f(source, AL_POSITION, 0, 0, 0);
	openal_error();
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	openal_error();
	alSourcei(source, AL_LOOPING, AL_FALSE);
	openal_error();

	alGenBuffers(1, &buffer);
	openal_error();

	openal_info(device);
	std::cout << "\n------------------------------\n";

	unsigned char* data = nullptr;
	ALsizei size;
	ALsizei freq;

	load_wav("audio/Kilogram of scotland_16.wav", &data, &size, &freq);
	std::cout << "Audio size: " << size << " Freq: " << freq << std::endl << std::endl;
	alBufferData(buffer, AL_FORMAT_STEREO16, data, size, freq);
	
	openal_error();

	alSourcei(source, AL_BUFFER, buffer);
	openal_error();
	alSourcePlay(source);
	openal_error();
	

	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	std::cout << "playing wav: " << (state == AL_PLAYING) << std::endl;
	do {
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	} while (state == AL_PLAYING);

	std::cout << "Stopped playing wav" << std::endl;

	getchar();
	return 0;

	//mp3 stuff

	ALuint sourcemp3;
	ALuint buffermp3;

	alGenSources((ALuint)1, &sourcemp3);
	alSourcef(sourcemp3, AL_PITCH, 1);
	//alSourcef(m_source, AL_GAIN, 1);
	alSource3f(sourcemp3, AL_POSITION, 0, 0, 0);
	alSource3f(sourcemp3, AL_VELOCITY, 0, 0, 0);
	alSourcei(sourcemp3, AL_LOOPING, AL_FALSE);

	alSourcef(sourcemp3, AL_ROLLOFF_FACTOR, 2.0f);
	alSourcef(sourcemp3, AL_REFERENCE_DISTANCE, 6);
	alSourcef(sourcemp3, AL_MAX_DISTANCE, 15);

	alGenBuffers((ALuint)1, &buffermp3);

	mp3dec_t mp3dec;
	mp3dec_file_info_t audioInfo;
	if (mp3dec_load(&mp3dec, "audio/kilogram-of-scotland.mp3", &audioInfo, NULL, NULL))
	{
		std::cout << "ERROR: Failed to load mp3" << std::endl;
		return 1;
	}

	alBufferData(buffermp3, AL_FORMAT_STEREO16, audioInfo.buffer, audioInfo.samples * sizeof(mp3d_sample_t), audioInfo.hz);
	alSourcei(sourcemp3, AL_BUFFER, buffermp3);

	//alSourcePlay(sourcemp3);

	alGetSourcei(sourcemp3, AL_SOURCE_STATE, &state);
	std::cout << "playing mp3: " << (state == AL_PLAYING) << std::endl;
	do {
		alGetSourcei(sourcemp3, AL_SOURCE_STATE, &state);
	} while (state == AL_PLAYING);

	std::cout << "Stopped playing mp3" << std::endl;

	getchar();

	return 0;
}

void openal_info(ALCdevice* dev)
{
	const ALchar* vendor = alGetString(AL_VENDOR);
	const ALchar* version = alGetString(AL_VERSION);
	const ALchar* renderer = alGetString(AL_RENDERER);
	const ALchar* openALExtensions = alGetString(AL_EXTENSIONS);
	const ALchar* ALCExtensions = alcGetString(dev, ALC_EXTENSIONS);
	//const ALchar* auxSends = alGetString(AL_MAX);
	printf("\tOpenAL info:\n\tVendor: %s\n\tVersion: %s\n\tRenderer: %s\n\tOpenAl Extensions: %s\n\tALC Extensions: %s\n",
		vendor, version, renderer, openALExtensions, ALCExtensions);
	ALCint srate;
	alcGetIntegerv(dev, ALC_FREQUENCY, 1, &srate);
	std::cout << "OpenAl device freq: " << srate << std::endl;
}

void setListener()
{
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	float orientation[] = { 0, 0, 1, 0, 1, 0 };
	alGetListenerfv(AL_ORIENTATION, orientation);
}

void openal_error()
{
	ALCenum error;
	error = alGetError();
	if (error != AL_NO_ERROR)
		std::cout << "ERROR: OpenAl error: " << error << std::endl;
}