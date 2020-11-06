#include <iostream>
#include <string>
#include <AL/al.h>
#include <AL/alc.h>
#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx.h>

#include "wav_loader.hpp"
#include "toneGenerator.hpp"

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
	const ALchar* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	const ALchar* defaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	std::cout << "Available devices: \n" << devices << std::endl;
	std::cout << "Default device: " << defaultDevice << std::endl;

	const ALchar* captureDevices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
	const ALchar* defaultCaptureDevice = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
	std::cout << "\nAvailable capture devices: \n" << captureDevices << std::endl;
	std::cout << "Default capture device: " << defaultCaptureDevice << std::endl;

	ALCdevice* device = alcOpenDevice(NULL);
	if (!device) std::cout << "Device failed to init" << std::endl;

	ALCint attribs[] = { ALC_FREQUENCY, 44100, 0 };
	ALCcontext* context = alcCreateContext(device, attribs);
	if (!alcMakeContextCurrent(context)) std::cout << "Context failed to be current" << std::endl;

	openal_error();
	setListener();

	openal_info(device);

	ALint resamplers = alGetInteger(AL_NUM_RESAMPLERS_SOFT);
	std::cout << "Resamplers: " << resamplers << std::endl;

	ALint auxSends;
	alcGetIntegerv(device, ALC_MAX_AUXILIARY_SENDS, 1, &auxSends);
	std::cout << "Aux sends available: " << auxSends << std::endl;

	for (int i = 0; i < resamplers; ++i)
	{
		std::cout << "\t[" << i << "]: " << alGetStringiSOFT(AL_RESAMPLER_NAME_SOFT, i);
	}

	ALint defaultResampler = alGetInteger(AL_DEFAULT_RESAMPLER_SOFT);
	std::cout << "\nDefault resampler: " << defaultResampler << " - " << alGetStringiSOFT(AL_RESAMPLER_NAME_SOFT, defaultResampler) << std::endl;


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
	alSourcei(source, AL_SOURCE_RESAMPLER_SOFT, 1);
	openal_error();


	/*ALuint effectSlot;
	alGenAuxiliaryEffectSlots(1, &effectSlot);
	ALuint effect;
	alGenEffects(1, &effect);

	alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
	openal_error();
	alEffectf(effect, AL_EAXREVERB_DECAY_TIME, 5.0f);
	openal_error();

	ALuint filter;
	alGenFilters(1, &filter);

	alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	openal_error();
	alFilteri(filter, AL_LOWPASS_GAIN, 0.5f);
	openal_error();
	alFilteri(filter, AL_LOWPASS_GAINHF, 0.5f);
	openal_error();

	alAuxiliaryEffectSloti(effectSlot, AL_EFFECTSLOT_EFFECT, effect);
	openal_error();
	

	alSource3i(source, AL_AUXILIARY_SEND_FILTER, effectSlot, 1, filter);
	openal_error();
	alSourcei(source, AL_DIRECT_FILTER, filter);
	openal_error();*/


	alGenBuffers(1, &buffer);
	openal_error();

	std::cout << "\n------------------------------\n";

	unsigned char* data = nullptr;
	ALsizei size;
	ALsizei freq = 192000;

	//load_wav("audio/kilogram-of-scotland_mono8.wav", &data, &size, &freq);
	generateTone(&data, &size, freq, 440, 1);
	std::cout << "Audio size: " << size << " Freq: " << freq << std::endl << std::endl;
	alBufferData(buffer, AL_FORMAT_MONO8, data, size, freq);
	openal_error();

	alSourcei(source, AL_BUFFER, buffer);
	openal_error();
	getchar();
	
	alSourcePlay(source);
	openal_error();
	

	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	std::cout << "playing: " << (state == AL_PLAYING) << std::endl;
	do {
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	} while (state == AL_PLAYING);

	std::cout << "Stopped playing" << std::endl;

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
	alListener3f(AL_POSITION, 0, 0, 1);
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