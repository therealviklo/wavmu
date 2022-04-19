#pragma once
#include "winerror.h"
#include "instrument.h"
#include "fs.h"
#include "cfg.h"

class SampleInstrument : public Instrument
{
private:
	Wave wave;
	Tone tone;
	std::string name;
	std::wstring dispName;
public:
	SampleInstrument(const char* file);
	
	Sample at(double pos, uint16_t channel, Tone tone) const override;

	const char* getName() const override;
	const wchar_t* getDispName() const override;
};