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
	EXCEPT(Exception)
private:
	struct {
		uint16_t channels;
		uint32_t sampleRate;
	} info;
	std::vector<Sample> data;
public:
	Wave(const char* file);

	uint16_t getChannels() const noexcept {return info.channels;}
	uint32_t getSampleRate() const noexcept {return info.sampleRate;}

	Sample at(double pos, uint16_t channel, double pitchShift) const;
};