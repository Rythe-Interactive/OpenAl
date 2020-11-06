#pragma once
#define PI 3.14159265358979
#include <cmath>

void generateTone(unsigned char** data, int* size, int sampleRate, int freq, int time = 1)
{
	*size = sampleRate * time;
	unsigned char* tone = new unsigned char[*size];
	for (size_t i = 0; i < *size; ++i)
	{
		float value = std::sin((i / (float)sampleRate)*2*PI * freq) * 127 + 128;
		tone[i] = static_cast<unsigned char>(value);
	}
	*data = new unsigned char[*size];
	memcpy(*data, tone, *size);
}