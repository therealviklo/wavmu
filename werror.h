#pragma once
#include <exception>
#include <memory>
#include <string>
#include <cwchar>

class Wexception : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "Wide exception";
	}

	virtual const wchar_t* wwhat() const noexcept
	{
		return L"Unknown exception";
	}
};

// WRE = Wide Runtime Exception
class WRE : public Wexception
{
	std::shared_ptr<wchar_t[]> str;
public:
	WRE(const wchar_t* msg)
	{
		const size_t len = std::wcslen(msg) + 1;
		str = std::make_shared<wchar_t[]>(len);
		std::wmemcpy(str.get(), msg, len);
	}

	WRE(const std::wstring& msg)
	{
		const size_t len = msg.length() + 1;
		str = std::make_shared<wchar_t[]>(len);
		std::wmemcpy(str.get(), msg.c_str(), len);
	}

	const wchar_t* wwhat() const noexcept override
	{
		return str.get();
	}
};