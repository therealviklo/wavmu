#pragma once
#include <cstdint>
#include <vector>
#include <numbers>
#include <mutex>
#include "codec.h"
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

	virtual const char* getName() const = 0;
	virtual const wchar_t* getDispName() const = 0;
};

extern const Instrument& defaultInstrument;