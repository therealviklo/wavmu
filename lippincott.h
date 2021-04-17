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
				L"Error",
				MB_ICONERROR
			);
		}
		catch (const WinError& e)
		{
			std::wostringstream ss;
			ss << e.wwhat()
			   << L" (Error code: 0x"
			   << std::hex << e.hr
			   << L")";
			MessageBoxW(
				nullptr,
				ss.str().c_str(),
				L"Error",
				MB_ICONERROR
			);
		}
		catch (const Wexception& e)
		{
			MessageBoxW(
				nullptr,
				e.wwhat(),
				L"Error",
				MB_ICONERROR
			);
		}
		catch (const std::exception& e)
		{
			MessageBoxW(
				nullptr,
				stringToWstring(e.what()).c_str(),
				L"Error",
				MB_ICONERROR
			);
		}
		catch (...)
		{
			MessageBoxW(
				nullptr,
				L"Unknown error",
				L"Error",
				MB_ICONERROR
			);
		}
	}
	catch (...)
	{
		MessageBoxW(
			nullptr,
			L"Exception thrown when trying to display error message",
			L"Error",
			MB_ICONERROR
		);
	}
}