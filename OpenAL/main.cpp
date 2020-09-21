#include <iostream>
#define MINIMP3_IMPLEMENTATION
#include "audio.hpp"

typedef ALboolean alBool;

static void list_audio_devices(const ALCchar* devices);
static void openal_error();
static mp3dec_file_info_t loadMp3(const char* path);

int main()
{

	Audio aud = Audio("audio/08 You're Dead.mp3");
	aud.play();
	
	while (true)
	{
		int in;
		std::cin >> in;
		if (in == 99) break;
	}

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

static mp3dec_file_info_t loadMp3(const char* path)
{
	mp3dec_t mp3d;
	mp3dec_file_info_t info;
	if (mp3dec_load(&mp3d, path, &info, NULL, NULL))
	{
		std::cout << "ERROR: Loading of Audio file went wrong\n";
	}
	return info;
}