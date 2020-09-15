#include "player.h"

void Player::loop()
{
	while (running)
	{

	}
}

Player::Player()
	: running(true),
	  playerThread(&Player::loop, this),
	  playing(false) {}

Player::~Player()
{
	running = false;
	playerThread.join();
}