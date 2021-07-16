#pragma once
#include <stdexcept>
#include <utility>
#include <string>
#include <sstream>
#include <type_traits>
#include <memory>
#include <concepts>
#include <cmath>
#include <vector>
#include <fstream>
#include "win.h"
#include "werror.h"

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
	if (!len) throw WRE(L"Failed to convert UTF-16 string to UTF-8 string");
	std::string ret(len, '\0');
	if (!WideCharToMultiByte(
		CP_UTF8,
		0,
		s.c_str(),
		s.length(),
		&ret[0],
		ret.length(),
		nullptr,
		nullptr
	)) throw WRE(L"Failed to convert UTF-16 string to UTF-8 string");
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
	if (!len) throw WRE(L"Failed to convert UTF-8 string to UTF-16 string");
	std::wstring ret(len, L'\0');
	if (!MultiByteToWideChar(
		CP_UTF8,
		0,
		s.c_str(),
		s.length(),
		&ret[0],
		ret.length()
	)) throw WRE(L"Failed to convert UTF-8 string to UTF-16 string");
	return ret;
}

template <typename T, auto Closer>
struct UHandleHelper
{
	constexpr inline void operator()(T ptr) const noexcept(noexcept(Closer(ptr)))
	{
		if (ptr) Closer(ptr);
	}
};
template <typename T, auto Closer>
	requires std::is_pointer_v<T>
using UHandle = std::unique_ptr<std::remove_pointer_t<T>, UHandleHelper<T, Closer>>;

inline std::vector<unsigned char> readfile(const char* file)
{
	std::ifstream fs;
	fs.exceptions(std::ios::failbit | std::ios::badbit);
	fs.open(file, std::ios::in | std::ios::binary);
	fs.seekg(0, std::ios::end);
	const size_t size = fs.tellg();
	fs.seekg(0, std::ios::beg);
	std::vector<unsigned char> data;
	data.resize(size);
	fs.read(reinterpret_cast<char*>(data.data()), size);
	return data;
}