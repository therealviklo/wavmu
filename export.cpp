#include "export.h"

void exportSong(Tracks& tracks, BPM bpm, const char* filename)
{
	size_t numsamples = 0;
	{
		std::lock_guard const lg(tracks.mtx);
		for (const auto& i : tracks.data)
		{
			for (const auto& j : i.sections)
			{
				numsamples = std::max<size_t>((j.timestamp + j.section->calcLength()) * 240 / bpm * 44100, numsamples);
			}
		}
	}
	const Wave::Info waveinfo{
		2,
		44100
	};
	std::vector<Sample> samples(numsamples * 2 /* kanaler */);
	{
		PlayState ps(tracks, bpm);
		for (size_t i = 0; i < numsamples; ++i)
		{
			const auto pair = ps.get(44100);
			samples[i * 2] = pair.samples[0];
			samples[i * 2 + 1] = pair.samples[1];
		}
	}
	encodeFile(filename, Wave(std::move(samples), waveinfo));
}