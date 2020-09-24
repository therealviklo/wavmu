#include "track.h"

PlayState::PlayState(Tracks& tracks)
{
	const std::lock_guard<std::mutex> lg(tracks.mtx);
	
	std::unique_lock<std::mutex> tmpUl(tracks.playingMutex, std::defer_lock);
	if (!tmpUl.try_lock()) throw AlreadyPlayingException();
	playingLock.swap(tmpUl);
}