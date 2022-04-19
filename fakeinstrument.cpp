#include "fakeinstrument.h"

FakeInstrument::FakeInstrument(std::string name) :
	name(name),
	dispName(stringToWstring(name)) {}

const char* FakeInstrument::getName() const
{
	return name.c_str();
}

const wchar_t* FakeInstrument::getDispName() const
{
	return dispName.c_str();
}