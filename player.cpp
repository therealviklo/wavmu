#include "player.h"

template <class T>
void voiceDeleter(T* voice)
{
	if (voice)
	{
		voice->DestroyVoice();
	}
}

void Player::Callback::OnBufferEnd(void* bufferContext) noexcept
{
	try
	{
		/* Vanligtvis borde player->playingWaiterMutex låsas men man ska tydligen
		   inte göra någon trådsynkronisering i en XAudio2-callback. */
		player->playingWaiter.notify_all();
	} catch (...) {}
}

void Player::loop()
{
	try
	{
		std::unique_lock<std::mutex> pwul(playingWaiterMutex);
		while (true)
		{
			playingWaiter.wait(pwul);

			{
				const std::lock_guard<std::mutex> lg(runningMutex);
				if (!running) break;
			}

			{
				std::unique_lock<std::mutex> ul(playStateMutex);
				if (playState)
				{
					ul.unlock();
					
					while (true)
					{
						{
							const std::lock_guard<std::mutex> lg(runningMutex);
							if (!running) break;
						}
						{
							const std::lock_guard<std::mutex> lg(comMutex);
							XAUDIO2_VOICE_STATE state;
							sourceVoice->GetState(&state);
							if (state.BuffersQueued >= sizeof(bufs) / sizeof(*bufs)) break;
						}

						{
							const std::lock_guard<std::mutex> lg(playStateMutex);
							for (size_t i = 0; i < sizeof(bufs[currBuf]) / sizeof(*bufs[currBuf]) / 2; i++)
							{
								auto samples = playState->get(44100);
								bufs[currBuf][2 * i] = samples.samples[0];
								bufs[currBuf][2 * i + 1] = samples.samples[1];
							}
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
	  masteringVoice(nullptr, &voiceDeleter<IXAudio2MasteringVoice>),
	  sourceVoice(nullptr, &voiceDeleter<IXAudio2SourceVoice>),
	  currBuf(0),
	  callback(this)
{
	if (FAILED(XAudio2Create(&xa2)))
		throw Exception("Failed to initialise XAudio2");

	IXAudio2MasteringVoice* tempMasteringVoice = nullptr;
	if (FAILED(xa2->CreateMasteringVoice(&tempMasteringVoice)))
		throw Exception("Failed to create mastering voice");
	decltype(masteringVoice) tempMasteringVoice2(tempMasteringVoice, &voiceDeleter<IXAudio2MasteringVoice>);
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
	decltype(sourceVoice) tempSourceVoice2(tempSourceVoice, &voiceDeleter<IXAudio2SourceVoice>);
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
		try
		{
			const std::lock_guard<std::mutex> lg(runningMutex);
			running = false;
		}
		catch (...)
		{
			running = false;
		}
		try
		{
			const std::lock_guard<std::mutex> lg(playingWaiterMutex);
			playingWaiter.notify_all();
		} catch (...) {}
		playerThread.join();
	} catch (...) {}
}

void Player::start(Tracks& tracks)
{
	{
		const std::lock_guard<std::mutex> lg(playStateMutex);
		playState = std::make_unique<PlayState>(tracks);
	}
	{
		const std::lock_guard<std::mutex> lg(playingWaiterMutex);
		playingWaiter.notify_all();
	}
}

void Player::stop()
{
	{
		const std::lock_guard<std::mutex> lg(playStateMutex);
		playState = nullptr;
	}
	{
		const std::lock_guard<std::mutex> lg(comMutex);
		sourceVoice->FlushSourceBuffers();
	}
}