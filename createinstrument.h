#pragma once
#include <cstdlib>
#include <filesystem>
#include "sininstrument.h"
#include "sampleinstrument.h"
#include "fakeinstrument.h"

std::unique_ptr<Instrument> createInstrument(const char* name);