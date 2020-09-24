#pragma once
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>

typedef int32_t Sample;

class Wave
{
	class Exception : public std::runtime_error
	{
	public:
		Exception(const char* msg) : std::runtime_error(msg) {}
	};
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