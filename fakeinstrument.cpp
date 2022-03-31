#include "fakeinstrument.h"

FakeInstrument::FakeInstrument(std::string name) :
	name(name) {}

std::string FakeInstrument::getName() const
{
	return name;
}