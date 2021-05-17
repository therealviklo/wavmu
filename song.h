#pragma once
#include <cstdint>
#include <vector>
#include <memory>

typedef uint8_t Tone;
typedef double BPM;

struct Note
{
	double timestamp;
	double length;
	Tone tone;
};

struct TimeSignature
{
	unsigned top; // Nämnare
	unsigned btm; // Täljare
};

struct Section
{
	std::vector<Note> notes;
	TimeSignature timesig{4u, 4u};
	/* double length; */ // Kanske

	double calcLength();
};
typedef std::vector<std::shared_ptr<Section>> Sections;