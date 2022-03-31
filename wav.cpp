#include "wav.h"

Wave::Wave(std::vector<Sample> data, Info info) :
	info(info),
	data(std::move(data)) {}

Sample Wave::at(double pos, uint16_t channel, double pitchShift) const
{
	const double samplePos = fmod(pos / pitchShift * info.sampleRate, data.size() / info.channels);
	return std::lerp(
		data.at(floor(samplePos) * info.channels + channel),
		data.at(floor(samplePos) * info.channels + channel + info.channels),
		fmod(samplePos, 1.0)
	);
}