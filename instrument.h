#pragma once
#include <cstdint>
#include <vector>
#include "wav.h"

class Instrument
{
public:
	~Instrument() = default;
};

class SampleInstrument : public Instrument
{
private:
	Wave wave;
public:
	SampleInstrument(const char* file);
};