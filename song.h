#pragma once
#include <cstdint>
#include <vector>

typedef uint8_t Tone;
typedef double BPM;

struct Note
{
	double timestamp;
	double length;
	Tone tone;
};

struct Section
{
	std::vector<Note> notes;
	/* double length; */ // Kanske
};
typedef std::vector<std::shared_ptr<Section>> Sections;