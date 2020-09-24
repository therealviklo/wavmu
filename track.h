#pragma once
#include <algorithm>
#include "instrument.h"

struct Track
{
	struct SectionRef
	{
		std::shared_ptr<Section> section;
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
	std::vector<Track> data;
};

class PlayState
{
public:
	class AlreadyPlayingException : std::runtime_error
	{
	public:
		AlreadyPlayingException() : std::runtime_error("Song is already playing") {}
	};
	struct NotePlayState
	{
		Tone tone;
		double timeElapsed;
		double duration;
	};
	struct SectionPlayState
	{
		std::vector<Note>::iterator iterator;
		std::vector<NotePlayState> currNotes;
	};
	struct SamplePair
	{
		Sample samples[2];
	};
private:
	std::unique_lock<std::mutex> playingLock;
	double position;
	Tracks& tracks;
	std::vector<std::vector<SectionPlayState>> trackIterators;
public:
	// Kastar PlayState::AlreadyPlayingException om låten redan spelas.
	PlayState(Tracks& tracks);

	SamplePair get(uint32_t sampleRate);
};