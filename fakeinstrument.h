#pragma once
#include "sininstrument.h"

class FakeInstrument : public SinInstrument
{
private:
	std::string name;
	std::wstring dispName;
public:
	FakeInstrument(std::string name);

	const char* getName() const override;
	const wchar_t* getDispName() const override;
};