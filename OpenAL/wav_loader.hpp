#pragma once
#include <cstdint>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>

typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint8_t uint8;
typedef const char* cstring;

bool compare(uint8 arr[], cstring string, int size);

/**
* Structs for storing WAV information
* Information of WAVE RIFF header format
* Information on this format can be found @ http://soundfile.sapp.org/doc/WaveFormat/
*/
struct RIFF_Header
{
	uint8 chunckId[4];
	int32 chunckSize;
	uint8 format[4];
};

struct WAVE_Format
{
	uint8 subChunckId[4];
	int32 subchunckSize;
	int16 audioFormat;
	int16 channels;
	int32 sampleRate;
	int32 byteRate;
	int16 blockAlign;
	int16 bitsPerSample;
};

struct WAVE_Data 
{
	uint8 subChunckId[4];
	int32 subChunck2Size;
};

void load_wav(cstring filePath, unsigned char** data, int* size, int* freq)
{
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	std::vector<uint8> fileData;

	{
		std::ifstream file;
		file.open(filePath, std::ifstream::in | std::ifstream::binary);
		if (!file)
		{
			std::cout << "Failed to load " << filePath << std::endl;
			return;
		}

		std::cout << "Loading audio file " << filePath << "..." << std::endl;

		// Load all the file into fileData;
		int loops = 0;
		while (!file.eof())
		{
			char* buffer = new char[1];
			file.read(buffer, 1);
			fileData.push_back(*buffer);
			++loops;
			delete buffer;
		}
		std::cout << "file loops: " << loops << std::endl;
		file.close();

		file.open(filePath, std::ifstream::ate);
		std::cout << "size " << file.tellg() << std::endl;
		file.close();
	}

	riff_header.chunckId[0] = fileData[0];
	riff_header.chunckId[1] = fileData[1];
	riff_header.chunckId[2] = fileData[2];
	riff_header.chunckId[3] = fileData[3];
	if (compare(riff_header.chunckId, "RIFF", 4))
	{
		std::cout << "Valid RIFF" << std::endl;
	}
	else return;

	riff_header.format[0] = fileData[8];
	riff_header.format[1] = fileData[9];
	riff_header.format[2] = fileData[10];
	riff_header.format[3] = fileData[11];
	if (compare(riff_header.format, "WAVE", 4))
	{
		std::cout << "Valid WAVE" << std::endl;
	}
	else return;

	wave_format.subChunckId[0] = fileData[12];
	wave_format.subChunckId[1] = fileData[13];
	wave_format.subChunckId[2] = fileData[14];
	wave_format.subChunckId[3] = fileData[15];
	if (compare(wave_format.subChunckId, "fmt ", 4))
	{
		std::cout << "Valid fmt " << std::endl;
	}
	else return;

	memcpy(&wave_format.subchunckSize, (void*)&fileData[16], 4);
	memcpy(&wave_format.audioFormat, (void*)&fileData[20], 2);
	memcpy(&wave_format.channels, (void*)&fileData[22], 2);
	memcpy(&wave_format.sampleRate, (void*)&fileData[24], 4);
	memcpy(&wave_format.byteRate, (void*)&fileData[28], 4);
	memcpy(&wave_format.blockAlign, (void*)&fileData[32], 2);
	memcpy(&wave_format.bitsPerSample, (void*)&fileData[34], 2);
	std::cout << "Chuncksize: " << wave_format.subchunckSize << std::endl;
	std::cout << "Audio format: " << wave_format.audioFormat << std::endl;
	std::cout << "Channels: " << wave_format.channels << std::endl;
	std::cout << "sampleRate: " << wave_format.sampleRate << std::endl;
	std::cout << "byteRate: " << wave_format.byteRate << std::endl;
	std::cout << "blockAlign: " << wave_format.blockAlign << std::endl;
	std::cout << "bitsPerSample: " << wave_format.bitsPerSample << std::endl;

	wave_data.subChunckId[0] = fileData[36];
	wave_data.subChunckId[1] = fileData[37];
	wave_data.subChunckId[2] = fileData[38];
	wave_data.subChunckId[3] = fileData[39];
	if (compare(wave_data.subChunckId, "data", 4))
	{
		std::cout << "Valid data" << std::endl;
	}
	else return;

	memcpy(&wave_data.subChunck2Size, (void*)&fileData[40], 4);
	std::cout << "subChunck2Size: " << wave_data.subChunck2Size << std::endl;

	if (true)
	{
		*data = new unsigned char[fileData.size() - 45];
		memcpy(*data, (void*)&fileData[44], fileData.size() - 45);
		/*for (int i = 0; i < fileData.size() - 44; ++i)
		{
			data[i] = fileData[44 + i];
			std::cout << "[" << i << "] " << data[i] << std::endl;
		}*/
		std::cout << "File Data size: " << fileData.size()-45 << std::endl;
		*size = (fileData.size() - 45);
		//std::cout << data << std::endl;
	}
	/*else
	{
		data = new unsigned char[wave_data.subChunck2Size];
		FILE* file;
		fopen_s(&file, filePath, "rb");
		fread(data, wave_data.subChunck2Size, 1, file);
		*size = wave_data.subChunck2Size;
	}*/
	*freq = wave_format.sampleRate;
	std::cout << "Loaded audio file!" << std::endl;
}

bool compare(uint8 arr[], cstring string, int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (arr[i] != string[i]) return false;
	}
	return true;
}