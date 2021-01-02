#pragma once
#include <thread>
#include <mutex>
#include "utils.h"
#include <xaudio2.h>
#include <wrl.h>
#include <stdexcept>
#include <memory>
#include <condition_variable>
#include "wav.h"
#include "track.h"
#include "threadutils.h"

using Microsoft::WRL::ComPtr;

template <class T>
void voiceDeleter(T* voice)
{
	voice->DestroyVoice();
}

class Player
{
public:
	EXCEPT(Exception)
private:
	std::atomic<bool> running;

	std::thread playerThread;

	Semaphore playingSemaphore;
	std::mutex playStateMutex;
	std::unique_ptr<PlayState> playState;

	std::mutex comMutex;
	ComPtr<IXAudio2> xa2;
	UHandle<IXAudio2MasteringVoice*, voiceDeleter<IXAudio2MasteringVoice>> masteringVoice;
	UHandle<IXAudio2SourceVoice*, voiceDeleter<IXAudio2SourceVoice>> sourceVoice;

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