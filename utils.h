#pragma once
#include <utility>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include "win.h"

// Diverse bra-att-ha-saker

/* Förlåt mig för att använda en så konstig makro men den här
   makron kan man använda för att snabbt definiera en exceptiontyp.
   Den har en konstruktor där man anger ett felmeddelande som visas. */
#define EXCEPT(name) \
	struct name final : public std::runtime_error \
	{ \
		name(const char* msg) : std::runtime_error(msg) {} \
	};

template <class Handle, auto Closer>
struct UHandleHelper
{
	constexpr inline void operator()(Handle h) noexcept(noexcept(Closer(nullptr)))
	{
		if (h) Closer(h);
	}
};
template <class Handle, auto Closer>
using UHandle = std::unique_ptr<std::remove_pointer_t<Handle>, UHandleHelper<Handle, Closer>>;

// Konvertera något till en std::string.
template <typename T>
std::string toString(T t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

// Konvertera något till en std::wstring.
template <typename T>
std::wstring toWString(T t)
{
	std::wstringstream ss;
	ss << t;
	return ss.str();
}

inline std::string wstringToString(const std::wstring& s)
{
	size_t len = WideCharToMultiByte(
		CP_UTF8,
		0,
		s.c_str(),
		s.length(),
		nullptr,
		0,
		nullptr,
		nullptr
	);
	std::string ret(len, '\0');
	WideCharToMultiByte(
		CP_UTF8,
		0,
		s.c_str(),
		s.length(),
		&ret[0],
		ret.length(),
		nullptr,
		nullptr
	);
	return ret;
}

inline std::wstring stringToWstring(const std::string& s)
{
	size_t len = MultiByteToWideChar(
		CP_UTF8,
		0,
		s.c_str(),
		s.length(),
		nullptr,
		0
	);
	std::wstring ret(len, L'\0');
	MultiByteToWideChar(
		CP_UTF8,
		0,
		s.c_str(),
		s.length(),
		&ret[0],
		ret.length()
	);
	return ret;
}