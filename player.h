#pragma once
#include <thread>
#include <mutex>
#include <xaudio2.h>
#include <wrl.h>
#include <stdexcept>
#include <memory>

using Microsoft::WRL::ComPtr;

template <class T>
void voiceDeleter(T* voice);

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
	size_t playingPos;

	std::mutex comMutex;
	ComPtr<IXAudio2> xa2;
	std::unique_ptr<IXAudio2MasteringVoice, decltype(&voiceDeleter<IXAudio2MasteringVoice>)> masteringVoice;
	std::unique_ptr<IXAudio2SourceVoice, decltype(&voiceDeleter<IXAudio2SourceVoice>)> sourceVoice;

	int32_t bufs[8][256];
	uint8_t currBuf;

	void loop();

	void startVoice();
	void stopVoice();
public:
	Player();
	~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void start();
	void stop();
};