#pragma once
#include <thread>
#include <mutex>
#include <xaudio2.h>

class Player
{
private:
	bool running;
	
	std::thread playerThread;

	bool playing;

	void loop();
public:
	Player();
	~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
};