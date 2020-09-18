#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

typedef ALboolean alBool;

static void list_audio_devices(const ALCchar* devices);
static void openal_error();
static 

int main()
{
	ALCdevice* device;

	device = alcOpenDevice(NULL);
	if (!device)
	{
		std::cout << "ERROR: ALC Device Error" << std::endl;
		return 1;
	}

	alBool enumeration;
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
	{
		std::cout << "Enumeration extension not supported" << std::endl;
	}
	else
	{
		// if enumaration extension is supported, devices can be listed
		std::cout << "Enumeration extension supported" << std::endl;
		// Passing NULL into alcGetString indicates that we want all devices
		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	}

	// Create an OpenAl context
	ALCcontext* openAl;
	// Pass in NULL because there is nothing special about our window
	openAl = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(openAl))
	{
		std::cout << "ERROR: Failed to create OpenAl context\n";
		openal_error();
	}

	// Define and configure listener
	ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);

	// Generate Source
	ALuint source;

	alGenSources((ALuint)1, &source);
	openal_error();
	alSourcef(source, AL_PITCH, 1);
	openal_error();
	alSourcef(source, AL_GAIN, 1);
	openal_error();
	alSource3f(source, AL_POSITION, 0,0,0);
	openal_error();
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	openal_error();
	alSourcei(source, AL_LOOPING, AL_FALSE);
	openal_error();

	// Gen Buffer
	ALuint buffer;

	alGenBuffers((ALuint)1, &buffer);
	openal_error();

	// Load an audio stream to buffer

	getchar();

	return 0;
}

static void list_audio_devices(const ALCchar* devices)
{
	// OpenAl devices list is seperated by a NULL character ('\0')
	// And terminated by two NULL characters
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	std::cout << "Devices list:\n";
	std::cout << "---------------\n";
	while (device && *device != NULL && next != NULL)
	{
		std::cout << device << "\n";
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	std::cout << "---------------\n";
}

static void openal_error()
{
	ALCenum error;
	error = alGetError();
	if (error != AL_NO_ERROR)
		std::cout << "ERROR: OpenAl error: " << error << std::endl;
}

#define MIMIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

static mp3dec_t loadMp3(const char* path)
{
	mp3dec_t mp3d;
	mp3dec_file_info_t info;
	if (mp3dec_load(&mp3d, path, &info, NULL, NULL))
	{
		std::cout << "ERROR: Loading of Audio file went wrong\n";
	}
	return mp3d;
}