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
	catch (const std::exception& e)
	{
		MessageBoxW(
			nullptr,
			(L"Varning: kunde inte öppna intrumentet \"" +
			stringToWstring(name) +
			L"\" (Fel: " +
			stringToWstring(e.what()) +
			L")").c_str(),
			L"Kunde inte öppna instrument",
			MB_ICONWARNING
		);
		return std::make_unique<FakeInstrument>(name);
	}
	catch (...)
	{
		MessageBoxW(
			nullptr,
			(L"Varning: kunde inte öppna intrumentet \"" +
			stringToWstring(name) +
			L"\"").c_str(),
			L"Kunde inte öppna instrument",
			MB_ICONWARNING
		);
		return std::make_unique<FakeInstrument>(name);
	}
}