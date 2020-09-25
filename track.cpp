#include "track.h"

PlayState::PlayState(Tracks& tracks, BPM bpm)
	: position(0),
	  tracks(tracks),
	  bpm(bpm)
{
	// Lås trackmutexen för vi ska ändra på tracks.
	const std::lock_guard<std::mutex> lg(tracks.mtx);
	
	// Försök att låsa "playing"-mutexen och swappa in den i playingLock.
	std::unique_lock<std::mutex> tmpUl(tracks.playingMutex, std::defer_lock);
	if (!tmpUl.try_lock()) throw AlreadyPlayingException();
	playingLock.swap(tmpUl);

	// Vi ska iterera igenom alla track nu så vi ändrar storleken på vectorn till antalet track.
	trackIterators.resize(tracks.data.size());
	// Gå igenom alla track.
	for (size_t i = 0; i < tracks.data.size(); i++)
	{
		auto& currTrack = tracks.data[i]; // Nuvarande elementet i tracks
		auto& currIteratorTrack = trackIterators[i]; // Nuvarande elementet i trackIterators

		// Ändra storleken på tracket till antalet sektioner.
		currIteratorTrack.resize(tracks.data[i].sections.size());
		// Gå igenom alla sektioner i tracket.
		for (size_t j = 0; j < currTrack.sections.size(); j++)
		{
			// Typ som currTrack men för sektionen (Jag avrefererar Track::SectionRef::section också)
			auto& currSection = *currTrack.sections[j].section;
			auto& currIteratorSection = currIteratorTrack[j]; // Som currIteratorTrack men för sektionen

			// Sortera alla noter i sektionen.
			std::sort(
				currSection.notes.begin(),
				currSection.notes.end(),
				[](const Note& l, const Note& r) -> bool {return l.timestamp < r.timestamp;}
			);

			// Sätt iteratorn till början av sektionen.
			currIteratorSection.iterator = currSection.notes.begin();
		}
	}
}

PlayState::SamplePair PlayState::get(uint32_t sampleRate)
{
	const std::lock_guard<std::mutex> lg(tracks.mtx);
	SamplePair sp{0, 0};

	position += 1.0 / sampleRate;

	for (size_t i = 0; i < trackIterators.size(); i++)
	{
		auto& currTrack = tracks.data[i];
		auto& currIteratorTrack = trackIterators[i];
		const Instrument* const currInstrument = currTrack.instrument ? currTrack.instrument.get() : &defaultInstrument;
		
		for (size_t j = 0; j < currIteratorTrack.size(); j++)
		{
			auto& currSection = currTrack.sections[j];
			auto& currIteratorSection = currIteratorTrack[j];

			for (auto& i : currIteratorSection.currNotes)
			{
				i.timeElapsed += 1.0 / sampleRate;
			}

			while (currIteratorSection.iterator != currSection.section->notes.end() &&
				   (currIteratorSection.iterator->timestamp + currSection.timestamp) / bpm * 240.0 <= position)
			{
				currIteratorSection.currNotes.push_back({
					currIteratorSection.iterator->tone,
					position - (currIteratorSection.iterator->timestamp + currSection.timestamp) / bpm * 240.0,
					currIteratorSection.iterator->length / bpm * 240.0
				});

				currIteratorSection.iterator++;
			}

			for (auto i = currIteratorSection.currNotes.begin(); i != currIteratorSection.currNotes.end(); )
			{
				if (i->timeElapsed > i->duration + currInstrument->getReleaseTime())
				{
					i = currIteratorSection.currNotes.erase(i);
					continue;
				}

				sp.samples[0] += currInstrument->at(i->timeElapsed, 0, i->tone)
								 * currInstrument->envelopeLevel(i->timeElapsed, i->duration);
				sp.samples[1] += currInstrument->at(i->timeElapsed, 1, i->tone)
								 * currInstrument->envelopeLevel(i->timeElapsed, i->duration);

				i++;
			}
		}
	}

	return sp;
}