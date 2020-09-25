#pragma once
#include <cstdint>
#include <vector>
#include <numbers>
#include <mutex>
#include "wav.h"
#include "song.h"
#include "envelope.h"

class Instrument
{
protected:
	Envelope envelope;
public:
	virtual ~Instrument() = default;

	virtual Sample at(double pos, uint16_t channel, Tone tone) const = 0;
	constexpr double envelopeLevel(double pos, double noteDuration) const {return envelope.at(pos, noteDuration);}

	constexpr double getReleaseTime() const noexcept {return envelope.release;}
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
	SinInstrument();

	Sample at(double pos, uint16_t channel, Tone tone) const override;
};