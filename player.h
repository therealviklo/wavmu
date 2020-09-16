#pragma once
#include <thread>
#include <mutex>
#include <xaudio2.h>
#include <wrl.h>
#include <stdexcept>

using Microsoft::WRL::ComPtr;

class Player
{
	class Exception : public std::runtime_error
	{
	public:
		Exception(const char* msg) : std::runtime_error(msg) {}
	};
private:
	std::mutex runningMutex;
	bool running;

	std::thread playerThread;

	std::mutex playingMutex;
	bool playing;

	ComPtr<IXAudio2> xa2;
	ComPtr<IXAudio2MasteringVoice> masteringVoice;

	void loop();
public:
	Player();
	~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
};