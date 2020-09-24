#pragma once
#include "instrument.h"

struct Track
{
	struct SectionRef
	{
		Section* section;
		double timestamp;
	};

	std::unique_ptr<Instrument> instrument;
	std::vector<SectionRef> sections;
};

struct Tracks
{
	// För att få tillgång till datan.
	std::mutex mtx;
	// Vissa saker ska inte göras när noterna spelas upp. Denna mutex är låst då.
	std::mutex playingMutex;
	std::vector<Track> tracks;
};

class PlayState
{
	class AlreadyPlayingException : std::runtime_error
	{
	public:
		AlreadyPlayingException() : std::runtime_error("Song is already playing") {}
	};
private:
	std::unique_lock<std::mutex> playingLock;
public:
	// Kastar PlayState::AlreadyPlayingException om låten redan spelas.
	PlayState(Tracks& tracks);
};