#pragma once
#include <shared_mutex>
#include <algorithm>
#include "instrument.h"
#include "threadutils.h"

struct SectionRef
{
	std::shared_ptr<Section> section;
	double timestamp;
};

struct Track
{
	std::unique_ptr<Instrument> instrument;
	std::vector<SectionRef> sections;
	double volume = 0.5;
};

struct Tracks
{
	SpecialReaderLock mtx;
	std::vector<Track> data;
};

class PlayState
{
public:
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
	SpecialLockGuard playingLock;
	double position;
	Tracks& tracks;
	std::vector<std::vector<SectionPlayState>> trackIterators;
	BPM bpm;
public:
	PlayState(Tracks& tracks, BPM bpm);

	SamplePair get(uint32_t sampleRate);
	void skip(double time);
	bool done();
};