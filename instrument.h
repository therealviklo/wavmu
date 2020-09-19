#pragma once
#include <cstdint>
#include <vector>

class Instrument
{
public:
	~Instrument() = default;
};

class SampleInstrument : public Instrument
{
private:
	std::vector<uint8_t> data;
public:
	SampleInstrument(const char* file);
};