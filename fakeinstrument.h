#pragma once
#include "sininstrument.h"

class FakeInstrument : public SinInstrument
{
private:
	std::string name;
public:
	FakeInstrument(std::string name);

	std::string getName() const override;
};