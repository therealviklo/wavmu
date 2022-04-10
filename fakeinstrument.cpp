#include "fakeinstrument.h"

FakeInstrument::FakeInstrument(std::string name) :
	name(name) {}

const char* FakeInstrument::getName() const
{
	return name.c_str();
}