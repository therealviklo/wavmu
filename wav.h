#pragma once
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include "utils.h"

typedef int32_t Sample;

class Wave
{
public:
	struct Info
	{
		uint16_t channels;
		uint32_t sampleRate;
	};
private:
	Info info;
	std::vector<Sample> data;
public:
	Wave(std::vector<Sample> data, Info info);

	uint16_t getChannels() const noexcept {return info.channels;}
	uint32_t getSampleRate() const noexcept {return info.sampleRate;}

	Sample at(double pos, uint16_t channel, double pitchShift) const;
	const std::vector<Sample>& getData() const noexcept { return data; }
};