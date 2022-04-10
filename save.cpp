#include "save.h"

void saveSong(Tracks& tracks, const char* filename)
{
	std::vector<unsigned char> data;
	auto write = [&](const auto& x) -> void {
		const size_t currSize = data.size();
		data.resize(currSize + sizeof(x));
		std::memcpy(&data[currSize], &x, sizeof(x));
	};
	{
		const std::unique_lock ul(tracks.mtx);

		// Versionsnummer
		write((std::uint64_t)1ull);

		std::map<const Section*, std::uint64_t> secNums;
		for (const Track& track : tracks.data)
		{
			for (const SectionRef& secref : track.sections)
			{
				if (!secNums.contains(secref.section.get()))
					secNums.emplace(secref.section.get(), secNums.size());
			}
		}

		write((std::uint64_t)secNums.size());
		for (const auto& i : secNums)
		{
			write(i.first->timesig);
			write((std::uint64_t)i.first->notes.size());
			for (const Note& note : i.first->notes)
			{
				write(note.length);
				write(note.timestamp);
				write(note.tone);
			}
		}

		write((std::uint64_t)tracks.data.size());
		for (const Track& track : tracks.data)
		{
			for (const char* instname = track.instrument->getName(); *instname != '\0'; instname++)
			{
				write(*instname);
			}
			write((char)'\0');
			write((std::uint64_t)track.sections.size());
			for (const SectionRef& secref : track.sections)
			{
				write(secref.timestamp);
				write(secNums[secref.section.get()]);
			}
		}
	}
	writefile(filename, data.data(), data.size());
}

void loadSong(Tracks& tracks, const char* filename)
{
	const std::vector<unsigned char> data = readfile(filename);
	size_t cur = 0;
	auto read = [&](auto t) {
		if (cur + sizeof(t) > data.size())
			throw WRE(L"Unexpected end of file");
		const size_t oldcur = cur;
		cur += sizeof(t);
		return *(decltype(t)*)&data[oldcur];
	};
	
	if (read(std::uint64_t()) != (std::uint64_t)1ull)
		throw WRE(L"Unknown file version");

	const std::uint64_t numSecs = read(std::uint64_t());
	std::vector<std::shared_ptr<Section>> secs;
	for (std::uint64_t i = 0; i < numSecs; i++)
	{
		std::shared_ptr<Section> sec = std::make_shared<Section>();
		sec->timesig = read(TimeSignature());
		const std::uint64_t numNotes = read(std::uint64_t());
		for (std::uint64_t j = 0; j < numNotes; j++)
		{
			sec->notes.emplace_back();
			sec->notes.rbegin()->length = read(double());
			sec->notes.rbegin()->timestamp = read(double());
			sec->notes.rbegin()->tone = read(Tone());
		}
		secs.push_back(sec);
	}

	std::vector<Track> newtracks;
	const std::uint64_t numTracks = read(std::uint64_t());
	newtracks.reserve(numTracks);
	for (std::uint64_t i = 0; i < numTracks; i++)
	{
		newtracks.emplace_back();

		std::string instname;
		while (char c = read(char()))
			instname += c;
		newtracks.rbegin()->instrument = createInstrument(instname.c_str());

		const std::uint64_t numTrackSecs = read(std::uint64_t());
		newtracks.rbegin()->sections.reserve(numTrackSecs);
		for (std::uint64_t j = 0; j < numTrackSecs; j++)
		{
			newtracks.rbegin()->sections.emplace_back();
			newtracks.rbegin()->sections.rbegin()->timestamp = read(double());
			newtracks.rbegin()->sections.rbegin()->section = secs[read(std::uint64_t())];
		}
	}

	const std::unique_lock ul(tracks.mtx);
	if (tracks.playing.test())
		throw WRE(L"Song is playing");
	tracks.data = std::move(newtracks);
}