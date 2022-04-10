#include "createinstrument.h"

std::unique_ptr<Instrument> createInstrument(const char* name)
{
	if (*name == '\0')
	{
		return std::make_unique<SinInstrument>();
	}

	try
	{
		return std::make_unique<SampleInstrument>(name);
	}
	catch (...)
	{
		lippincottNonFatal(
			L"Varning: kunde inte öppna intrumentet \"" +
			stringToWstring(name) +
			L"\"",
			MB_ICONWARNING
		);
		return std::make_unique<FakeInstrument>(name);
	}
}