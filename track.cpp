#include "track.h"

PlayState::PlayState(Tracks& tracks)
	: position(0),
	  tracks(tracks.data)
{
	const std::lock_guard<std::mutex> lg(tracks.mtx);
	
	std::unique_lock<std::mutex> tmpUl(tracks.playingMutex, std::defer_lock);
	if (!tmpUl.try_lock()) throw AlreadyPlayingException();
	playingLock.swap(tmpUl);

	trackIterators.resize(tracks.data.size());
	for (size_t i = 0; i < tracks.data.size(); i++)
	{
		std::sort(
			tracks.data[i].sections.begin(),
			tracks.data[i].sections.end(),
			[](const Track::SectionRef& l, const Track::SectionRef& r) -> bool {return l.timestamp < r.timestamp;}
		);

		trackIterators[i].resize(tracks.data[i].sections.size());
		for (size_t j = 0; j < tracks.data[i].sections.size(); j++)
		{
			std::sort(
				tracks.data[i].sections[j].section->notes.begin(),
				tracks.data[i].sections[j].section->notes.end(),
				[](const Note& l, const Note& r) -> bool {return l.timestamp < r.timestamp;}
			);

			trackIterators[i][j].iterator = tracks.data[i].sections[j].section->notes.begin();
		}
	}
}

PlayState::SamplePair PlayState::get(uint32_t sampleRate) const
{
	SamplePair sp{0, 0};

	return sp;
}