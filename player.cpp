#include "player.h"

template <class T>
void voiceDeleter(T* voice)
{
	if (voice)
	{
		voice->DestroyVoice();
	}
}

void Player::loop()
{
	try
	{
		while (true)
		{
			{
				const std::lock_guard<std::mutex> lg(runningMutex);
				if (!running) break;
			}

			{
				const std::lock_guard<std::mutex> lg(playingMutex);
				if (playing)
				{
					{
						const std::lock_guard<std::mutex> lg2(comMutex);
						XAUDIO2_VOICE_STATE state;
						sourceVoice->GetState(&state);
						if (state.BuffersQueued >= sizeof(bufs) / sizeof(*bufs)) continue;
					}

					for (size_t i = 0; i < sizeof(bufs[currBuf]) / sizeof(*bufs[currBuf]); i++)
					{
						bufs[currBuf][i] = sin(playingPos / 50.0) * ((int32_t)(~(uint32_t)0 >> 1) / 2);
						playingPos++;
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
	playingPos = 0;
}

void Player::stopVoice()
{
	const std::lock_guard<std::mutex> lg(comMutex);
	if (FAILED(sourceVoice->Stop()))
		throw Exception("Failed to stop source voice");
}

Player::Player()
	: running(true),
	  playing(false),
	  playingPos(0),
	  masteringVoice(nullptr, &voiceDeleter<IXAudio2MasteringVoice>),
	  sourceVoice(nullptr, &voiceDeleter<IXAudio2SourceVoice>),
	  currBuf(0)
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
	if (FAILED(xa2->CreateSourceVoice(&tempSourceVoice, &wf)))
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
		playerThread.join();
	} catch (...) {}
}

void Player::start()
{
	const std::lock_guard<std::mutex> lg(playingMutex);
	playing = true;
}

void Player::stop()
{
	const std::lock_guard<std::mutex> lg(playingMutex);
	playing = false;
}