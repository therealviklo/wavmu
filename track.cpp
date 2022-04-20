#include "track.h"

PlayState::PlayState(Tracks& tracks, BPM bpm) :
	playingLock(tracks.mtx),
	position(0),
	tracks(tracks),
	bpm(bpm)
{
	UpgradedLockGuard el(tracks.mtx);

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

void advanceSection(SectionRef& currSection, PlayState::SectionPlayState& currIteratorSection, double time, BPM bpm, double position)
{
	for (auto& i : currIteratorSection.currNotes)
	{
		i.timeElapsed += time;
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
}

/* Hoppas att du inte behöver ändra den här funktionen för
   då behöver du kanske ändra PlayState::skip() också ;) */
PlayState::SamplePair PlayState::get(uint32_t sampleRate)
{
	SamplePair sp{0, 0};

	position += 1.0 / sampleRate;

	for (size_t i = 0; i < trackIterators.size(); i++)
	{
		auto& currTrack = tracks.data[i];
		auto& currIteratorTrack = trackIterators[i];
		const Instrument* const currInstrument = currTrack.instrument ? currTrack.instrument.get() : &defaultInstrument;
		
		const double vol = 4.0 * tracks.data[i].volume * tracks.data[i].volume;

		for (size_t j = 0; j < currIteratorTrack.size(); j++)
		{
			auto& currSection = currTrack.sections[j];
			auto& currIteratorSection = currIteratorTrack[j];

			advanceSection(currSection, currIteratorSection, 1.0 / sampleRate, bpm, position);

			for (auto i = currIteratorSection.currNotes.begin(); i != currIteratorSection.currNotes.end(); )
			{
				if (i->timeElapsed > i->duration + currInstrument->getReleaseTime())
				{
					i = currIteratorSection.currNotes.erase(i);
					continue;
				}

				sp.samples[0] += currInstrument->at(i->timeElapsed, 0, i->tone)
								* currInstrument->envelopeLevel(i->timeElapsed, i->duration)
								* vol;
				sp.samples[1] += currInstrument->at(i->timeElapsed, 1, i->tone)
								* currInstrument->envelopeLevel(i->timeElapsed, i->duration)
								* vol;

				i++;
			}
		}
	}

	return sp;
}

/* Hoppas att du inte behöver ändra den här funktionen för
   då behöver du kanske ändra PlayState::get() också ;) */
void PlayState::skip(double time)
{
	position += time;

	for (size_t i = 0; i < trackIterators.size(); i++)
	{
		auto& currTrack = tracks.data[i];
		auto& currIteratorTrack = trackIterators[i];
		const Instrument* const currInstrument = currTrack.instrument ? currTrack.instrument.get() : &defaultInstrument;
		
		for (size_t j = 0; j < currIteratorTrack.size(); j++)
		{
			auto& currSection = currTrack.sections[j];
			auto& currIteratorSection = currIteratorTrack[j];
			
			advanceSection(currSection, currIteratorSection, time, bpm, position);

			for (auto i = currIteratorSection.currNotes.begin(); i != currIteratorSection.currNotes.end(); )
			{
				if (i->timeElapsed > i->duration + currInstrument->getReleaseTime())
				{
					i = currIteratorSection.currNotes.erase(i);
					continue;
				}

				i++;
			}
		}
	}
}

bool PlayState::done()
{
	for (size_t i = 0; i < trackIterators.size(); i++)
	{
		auto& currTrack = tracks.data[i];
		auto& currIteratorTrack = trackIterators[i];
		
		for (size_t j = 0; j < currIteratorTrack.size(); j++)
		{
			auto& currSection = currTrack.sections[j];
			auto& currIteratorSection = currIteratorTrack[j];

			if (currIteratorSection.iterator != currSection.section->notes.end())
				return false;
			if (!currIteratorSection.currNotes.empty())
				return false;
		}
	}
	return true;
}