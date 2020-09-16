#include "player.h"

void Player::loop()
{
	try
	{
		while (true)
		{
			{
				std::lock_guard lg(runningMutex);
				if (!running) break;
			}

			bool playingLocal;
			{
				std::lock_guard lg(playingMutex);
				playingLocal = playing;
			}
			if (playingLocal)
			{

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

Player::Player()
	: running(true),
	  playing(false)
{
	if (FAILED(XAudio2Create(&xa2)))
		throw Exception("Failed to initialise XAudio2");

	if (FAILED(xa2->CreateMasteringVoice(&masteringVoice)))
		throw Exception("Failed to create mastering voice");

	std::thread tempThread(&Player::loop, this);
	playerThread.swap(tempThread);
}

Player::~Player()
{
	try
	{
		try
		{
			std::lock_guard lg(runningMutex);
			running = false;
		}
		catch (...)
		{
			running = false;
		}
		playerThread.join();
	} catch (...) {}
}