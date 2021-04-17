#include "player.h"

void Player::Callback::OnBufferEnd(void* /*bufferContext*/) noexcept
{
	player->playingWaiter.requestWakeup();
}

void Player::loop()
{
	try
	{
		while (true)
		{
			playingWaiter.wait();

			if (!running.load(std::memory_order_relaxed)) break;

			{
				std::unique_lock<std::mutex> ul(playStateMutex);
				if (playState)
				{
					while (true)
					{
						if (!running.load(std::memory_order_relaxed)) break;
						{
							const std::lock_guard<std::mutex> lg(comMutex);
							XAUDIO2_VOICE_STATE state;
							sourceVoice->GetState(&state);
							if (state.BuffersQueued >= sizeof(bufs) / sizeof(*bufs)) break;
						}

						for (size_t i = 0; i < sizeof(bufs[currBuf]) / sizeof(*bufs[currBuf]) / 2; i++)
						{
							auto samples = playState->get(44100);
							bufs[currBuf][2 * i] = samples.samples[0];
							bufs[currBuf][2 * i + 1] = samples.samples[1];
						}

						{
							const std::lock_guard<std::mutex> lg2(comMutex);
							XAUDIO2_BUFFER bd{};
							bd.AudioBytes = sizeof(bufs[currBuf]);
							bd.pAudioData = reinterpret_cast<uint8_t*>(&bufs[currBuf]);
							if (FAILED(sourceVoice->SubmitSourceBuffer(&bd)))
								throw Exception("Failed to submit source buffer");
						}

						currBuf = (currBuf + 1) % (sizeof(bufs) / sizeof(*bufs));
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Sound player error", MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxW(nullptr, L"Unknown error.", L"Sound player error", MB_ICONERROR);
	}
}

void Player::startVoice()
{
	const std::lock_guard<std::mutex> lg(comMutex);
	if (FAILED(sourceVoice->Start()))
		throw Exception("Failed to start source voice");
}

void Player::stopVoice()
{
	const std::lock_guard<std::mutex> lg(comMutex);
	if (FAILED(sourceVoice->Stop()))
		throw Exception("Failed to stop source voice");
	sourceVoice->FlushSourceBuffers();
}

Player::Player()
	: running(true),
	  masteringVoice(nullptr),
	  sourceVoice(nullptr),
	  currBuf(0),
	  callback(this)
{
	if (FAILED(XAudio2Create(&xa2)))
		throw Exception("Failed to initialise XAudio2");

	IXAudio2MasteringVoice* tempMasteringVoice = nullptr;
	if (FAILED(xa2->CreateMasteringVoice(&tempMasteringVoice)))
		throw Exception("Failed to create mastering voice");
	decltype(masteringVoice) tempMasteringVoice2(tempMasteringVoice);
	tempMasteringVoice2.swap(masteringVoice);

	WAVEFORMATEX wf{};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 2;
	wf.nSamplesPerSec = 44100;
	wf.nBlockAlign = 2 * sizeof(**bufs);
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.wBitsPerSample = sizeof(**bufs) * 8;
	IXAudio2SourceVoice* tempSourceVoice = nullptr;
	if (FAILED(xa2->CreateSourceVoice(&tempSourceVoice, &wf, 0, 2.0f, &callback)))
		throw Exception("Failed to create source voice");
	decltype(sourceVoice) tempSourceVoice2(tempSourceVoice);
	tempSourceVoice2.swap(sourceVoice);

	startVoice();

	std::thread tempThread(&Player::loop, this);
	playerThread.swap(tempThread);
}

Player::~Player()
{
	try
	{
		stopVoice();
	} catch (...) {}
	try
	{
		running.store(false, std::memory_order_relaxed);
		playingWaiter.requestWakeup();
		playerThread.join();
	} catch (...) {}
}

void Player::start(Tracks& tracks, BPM bpm)
{
	{
		const std::lock_guard<std::mutex> lg(playStateMutex);
		playState.reset();
		playState = std::make_unique<PlayState>(tracks, bpm);
	}
	playingWaiter.requestWakeup();
}

void Player::stop()
{
	{
		const std::lock_guard<std::mutex> lg(playStateMutex);
		playState.reset();
	}
	{
		const std::lock_guard<std::mutex> lg(comMutex);
		sourceVoice->FlushSourceBuffers();
	}
}