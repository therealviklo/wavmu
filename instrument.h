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
	~Instrument() = default;

	virtual Sample at(uint32_t sampleNum, uint32_t sampleRate, uint16_t channel, Tone tone) const = 0;
};

class SampleInstrument : public Instrument
{
private:
	Wave wave;
	Tone tone;
public:
	SampleInstrument(const char* file);
	
	Sample at(uint32_t sampleNum, uint32_t sampleRate, uint16_t channel, Tone tone) const override;
};

class SinInstrument : public Instrument
{
public:
	Sample at(uint32_t sampleNum, uint32_t sampleRate, uint16_t channel, Tone tone) const override;
};