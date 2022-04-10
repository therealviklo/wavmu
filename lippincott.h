#pragma once
#include <exception>
#include <sstream>
#include "winerror.h"
#include "utils.h"

inline void lippincott() noexcept
{
	if (std::current_exception() == nullptr) return;

	try
	{
		try
		{
			throw;
		}
		catch (const NoResWinError& e)
		{
			MessageBoxW(
				nullptr,
				e.wwhat(),
				L"Fatalt fel",
				MB_ICONERROR
			);
		}
		catch (const WinError& e)
		{
			std::wostringstream ss;
			ss << e.wwhat()
			   << L" (Felkod: 0x"
			   << std::hex << e.hr
			   << L")";
			MessageBoxW(
				nullptr,
				ss.str().c_str(),
				L"Fatalt fel",
				MB_ICONERROR
			);
		}
		catch (const Wexception& e)
		{
			MessageBoxW(
				nullptr,
				e.wwhat(),
				L"Fatalt fel",
				MB_ICONERROR
			);
		}
		catch (const std::exception& e)
		{
			MessageBoxW(
				nullptr,
				stringToWstring(e.what()).c_str(),
				L"Fatalt fel",
				MB_ICONERROR
			);
		}
		catch (...)
		{
			MessageBoxW(
				nullptr,
				L"Okänt fel",
				L"Fatalt fel",
				MB_ICONERROR
			);
		}
	}
	catch (...)
	{
		MessageBoxW(
			nullptr,
			L"Fel uppstod när felmeddelande skulle visas",
			L"Fatalt fel",
			MB_ICONERROR
		);
	}
}

inline void lippincottNonFatal(const std::wstring& msg, UINT icon = MB_ICONERROR)
{
	try
	{
		throw;
	}
	catch (const WinError& e)
	{
		std::wostringstream ss;
		ss << msg
			<< L" (Fel: "
			<< e.wwhat()
			<< L", Felkod: 0x"
			<< std::hex << e.hr
			<< L")";
		MessageBoxW(
			nullptr,
			ss.str().c_str(),
			L"Fel",
			icon
		);
	}
	catch (const Wexception& e)
	{
		MessageBoxW(
			nullptr,
			(msg +
			L" (Fel: "s +
			e.wwhat() +
			L")").c_str(),
			L"Fel",
			icon
		);
	}
	catch (const std::exception& e)
	{
		MessageBoxW(
			nullptr,
			(msg +
			L" (Fel: "s +
			stringToWstring(e.what()) +
			L")").c_str(),
			L"Fel",
			icon
		);
	}
	catch (...)
	{
		MessageBoxW(
			nullptr,
			msg.c_str(),
			L"Fel",
			icon
		);
	}
}