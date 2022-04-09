#pragma once
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <filesystem>
#include <cctype>
#include <bit>
#include "utils.h"
#include "wav.h"

typedef int32_t Sample;

namespace Decode
{
	Wave wave(const unsigned char* data, size_t size);
}

namespace Encode
{
	std::vector<unsigned char> wave(const Wave& wave);
}

Wave decodeFile(const char* file);
void encodeFile(const char* file, const Wave& wave);