#pragma once
#include <cstdint>
#include <vector>
#include <numbers>
#include <mutex>
#include "wav.h"
#include "song.h"

class Instrument
{
public:
	virtual ~Instrument() = default;

	virtual Sample at(double pos, uint16_t channel, Tone tone) const = 0;
};

class SampleInstrument : public Instrument
{
private:
	Wave wave;
	Tone tone;
public:
	SampleInstrument(const char* file);
	
	Sample at(double pos, uint16_t channel, Tone tone) const override;
};

class SinInstrument : public Instrument
{
public:
	Sample at(double pos, uint16_t channel, Tone tone) const override;
};