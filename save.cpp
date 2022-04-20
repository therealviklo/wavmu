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
		const std::shared_lock sl(tracks.mtx);

		// Versionsnummer
		write((std::uint64_t)3ull);

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
			write(i.second);
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
			write(track.volume);
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

template <typename T>
T read(size_t& cur, const std::vector<unsigned char>& data)
{
	if (cur + sizeof(T) > data.size())
		throw WRE(L"Unexpected end of file");
	const size_t oldcur = cur;
	cur += sizeof(T);
	return *(T*)&data[oldcur];
}

void loadSong1(Tracks& tracks, size_t oldcur, const std::vector<unsigned char>& data)
{
	size_t cur = oldcur;

	const std::uint64_t numSecs = read<std::uint64_t>(cur, data);
	std::vector<std::shared_ptr<Section>> secs;
	for (std::uint64_t i = 0; i < numSecs; i++)
	{
		std::shared_ptr<Section> sec = std::make_shared<Section>();
		sec->timesig = read<TimeSignature>(cur, data);
		const std::uint64_t numNotes = read<std::uint64_t>(cur, data);
		for (std::uint64_t j = 0; j < numNotes; j++)
		{
			sec->notes.emplace_back();
			sec->notes.rbegin()->length = read<double>(cur, data);
			sec->notes.rbegin()->timestamp = read<double>(cur, data);
			sec->notes.rbegin()->tone = read<Tone>(cur, data);
		}
		secs.push_back(sec);
	}

	std::vector<Track> newtracks;
	const std::uint64_t numTracks = read<std::uint64_t>(cur, data);
	newtracks.reserve(numTracks);
	for (std::uint64_t i = 0; i < numTracks; i++)
	{
		newtracks.emplace_back();

		newtracks.rbegin()->volume = 0.5;

		std::string instname;
		while (char c = read<char>(cur, data))
			instname += c;
		newtracks.rbegin()->instrument = createInstrument(instname.c_str());

		const std::uint64_t numTrackSecs = read<std::uint64_t>(cur, data);
		newtracks.rbegin()->sections.reserve(numTrackSecs);
		for (std::uint64_t j = 0; j < numTrackSecs; j++)
		{
			newtracks.rbegin()->sections.emplace_back();
			newtracks.rbegin()->sections.rbegin()->timestamp = read<double>(cur, data);
			newtracks.rbegin()->sections.rbegin()->section = secs[read<std::uint64_t>(cur, data)];
		}
	}

	std::unique_lock ul(tracks.mtx, std::defer_lock);
	if (!ul.try_lock())
		throw WRE(L"Song is playing");
	tracks.data = std::move(newtracks);
}

void loadSong2(Tracks& tracks, size_t oldcur, const std::vector<unsigned char>& data)
{
	size_t cur = oldcur;

	const std::uint64_t numSecs = read<std::uint64_t>(cur, data);
	std::vector<std::shared_ptr<Section>> secs;
	for (std::uint64_t i = 0; i < numSecs; i++)
	{
		std::shared_ptr<Section> sec = std::make_shared<Section>();
		sec->timesig = read<TimeSignature>(cur, data);
		const std::uint64_t numNotes = read<std::uint64_t>(cur, data);
		for (std::uint64_t j = 0; j < numNotes; j++)
		{
			sec->notes.emplace_back();
			sec->notes.rbegin()->length = read<double>(cur, data);
			sec->notes.rbegin()->timestamp = read<double>(cur, data);
			sec->notes.rbegin()->tone = read<Tone>(cur, data);
		}
		secs.push_back(sec);
	}

	std::vector<Track> newtracks;
	const std::uint64_t numTracks = read<std::uint64_t>(cur, data);
	newtracks.reserve(numTracks);
	for (std::uint64_t i = 0; i < numTracks; i++)
	{
		newtracks.emplace_back();

		newtracks.rbegin()->volume = read<double>(cur, data);

		std::string instname;
		while (char c = read<char>(cur, data))
			instname += c;
		newtracks.rbegin()->instrument = createInstrument(instname.c_str());

		const std::uint64_t numTrackSecs = read<std::uint64_t>(cur, data);
		newtracks.rbegin()->sections.reserve(numTrackSecs);
		for (std::uint64_t j = 0; j < numTrackSecs; j++)
		{
			newtracks.rbegin()->sections.emplace_back();
			newtracks.rbegin()->sections.rbegin()->timestamp = read<double>(cur, data);
			newtracks.rbegin()->sections.rbegin()->section = secs[read<std::uint64_t>(cur, data)];
		}
	}

	std::unique_lock ul(tracks.mtx, std::defer_lock);
	if (!ul.try_lock())
		throw WRE(L"Song is playing");
	tracks.data = std::move(newtracks);
}

void loadSong3(Tracks& tracks, size_t oldcur, const std::vector<unsigned char>& data)
{
	size_t cur = oldcur;

	const std::uint64_t numSecs = read<std::uint64_t>(cur, data);
	std::map<std::uint64_t, std::shared_ptr<Section>> secs;
	for (std::uint64_t i = 0; i < numSecs; i++)
	{
		const std::uint64_t id = read<std::uint64_t>(cur, data);
		std::shared_ptr<Section> sec = std::make_shared<Section>();
		sec->timesig = read<TimeSignature>(cur, data);
		const std::uint64_t numNotes = read<std::uint64_t>(cur, data);
		for (std::uint64_t j = 0; j < numNotes; j++)
		{
			sec->notes.emplace_back();
			sec->notes.rbegin()->length = read<double>(cur, data);
			sec->notes.rbegin()->timestamp = read<double>(cur, data);
			sec->notes.rbegin()->tone = read<Tone>(cur, data);
		}
		secs.emplace(id, std::move(sec));
	}

	std::vector<Track> newtracks;
	const std::uint64_t numTracks = read<std::uint64_t>(cur, data);
	newtracks.reserve(numTracks);
	for (std::uint64_t i = 0; i < numTracks; i++)
	{
		newtracks.emplace_back();

		newtracks.rbegin()->volume = read<double>(cur, data);

		std::string instname;
		while (char c = read<char>(cur, data))
			instname += c;
		newtracks.rbegin()->instrument = createInstrument(instname.c_str());

		const std::uint64_t numTrackSecs = read<std::uint64_t>(cur, data);
		newtracks.rbegin()->sections.reserve(numTrackSecs);
		for (std::uint64_t j = 0; j < numTrackSecs; j++)
		{
			newtracks.rbegin()->sections.emplace_back();
			newtracks.rbegin()->sections.rbegin()->timestamp = read<double>(cur, data);
			newtracks.rbegin()->sections.rbegin()->section = secs[read<std::uint64_t>(cur, data)];
		}
	}

	std::unique_lock ul(tracks.mtx, std::defer_lock);
	if (!ul.try_lock())
		throw WRE(L"Song is playing");
	tracks.data = std::move(newtracks);
}

void loadSong(Tracks& tracks, const char* filename)
{
	const std::vector<unsigned char> data = readfile(filename);
	size_t cur = 0;
	
	switch (read<std::uint64_t>(cur, data))
	{
		case 1ull:
		{
			loadSong1(tracks, cur, data);
		}
		break;
		case 2ull:
		{
			loadSong2(tracks, cur, data);
		}
		break;
		case 3ull:
		{
			loadSong3(tracks, cur, data);
		}
		break;
		default:
			throw WRE(L"Unknown file version");
	}
}