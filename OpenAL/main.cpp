#include <iostream>
#include <string>
#define MINIMP3_IMPLEMENTATION
#include "audio.hpp"

int main()
{
	// The sound files used in this example are property of Konami
	// The original sound was first released on 
	// Castlevania on the Nintendo Entertainment System [NES]
	Audio music[] = {
		Audio("audio/01 introduction (Castle Gate).mp3"),
		Audio("audio/02 Vampire Killer (courtyard).mp3"),
		Audio("audio/12 Stage Clear.mp3"),
		Audio("audio/10 Black Night (Final Battle).mp3"),
		Audio("audio/14 Death.mp3"),
		Audio("audio/13 All Clear.mp3")
	};

	int order[] = { 0, 1, 2, 3, 4, 3, 5 };

	int musicCount = 7;

	std::cout << "press return to start!";
	getchar();
	std::cout << "Audio start" << std::endl;

	music[0].play();

	int current = 0;

	while (true)
	{
		std::string in;
		std::getline(std::cin, in);
		if (in == "quit" || in == "exit")
		{
			music[order[current]].stop();
			break;
		}
		else if (in == "pause") music[order[current]].pause();
		else if (in == "play") music[order[current]].play();
		else if (in == "pitch") music[order[current]].setPitch(1.0f);
		else if (in == "pitch+") music[order[current]].setPitch(music[order[current]].getPitch() + 0.05f);
		else if (in == "pitch++") music[order[current]].setPitch(music[order[current]].getPitch() + 0.2f);
		else if (in == "pitch-") music[order[current]].setPitch(music[order[current]].getPitch() - 0.05f);
		else if (in == "pitch--") music[order[current]].setPitch(music[order[current]].getPitch() - 0.2f);
		else if (in == "next")
		{
			music[order[current]].stop();
			current = ++current;
			if (current == musicCount) break;
			music[order[current]].play();
		}
	}

	std::cout << "\n----------------------------\nDone playing\n";
	
	getchar();

	return 0;
}