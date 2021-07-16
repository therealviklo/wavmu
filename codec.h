#pragma once
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <filesystem>
#include <cctype>
#include "utils.h"
#include "wav.h"

typedef int32_t Sample;

namespace Decoder
{
	Wave wave(const unsigned char* data, size_t size);
}

Wave decodeFile(const char* file);