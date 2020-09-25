#pragma once
#include <thread>
#include <mutex>
#include <xaudio2.h>
#include <wrl.h>
#include <stdexcept>
#include <memory>
#include <condition_variable>
#include "wav.h"
#include "track.h"

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

	std::mutex playingWaiterMutex;
	std::condition_variable playingWaiter;
	std::mutex playStateMutex;
	std::unique_ptr<PlayState> playState;

	std::mutex comMutex;
	ComPtr<IXAudio2> xa2;
	std::unique_ptr<IXAudio2MasteringVoice, decltype(&voiceDeleter<IXAudio2MasteringVoice>)> masteringVoice;
	std::unique_ptr<IXAudio2SourceVoice, decltype(&voiceDeleter<IXAudio2SourceVoice>)> sourceVoice;

	Sample bufs[8][256];
	uint8_t currBuf;

	class Callback : public IXAudio2VoiceCallback
	{
	private:
		Player* player;
	public:
		Callback(Player* player) : player(player) {}

		void OnBufferEnd(void* bufferContext) noexcept override;
		void OnBufferStart(void* bufferContext) noexcept override {}
		void OnLoopEnd(void* bufferContext) noexcept override {}
		void OnStreamEnd() noexcept override {}
		void OnVoiceError(void* bufferContext, HRESULT error) noexcept override {}
		void OnVoiceProcessingPassEnd() noexcept override {}
		void OnVoiceProcessingPassStart(UINT32 bytesRequired) noexcept override {}
	} callback;

	void loop();

	void startVoice();
	void stopVoice();
public:
	Player();
	~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void start(Tracks& tracks, BPM bpm);
	void stop();
};