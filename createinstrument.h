#pragma once
#include <cstdlib>
#include "sininstrument.h"
#include "sampleinstrument.h"
#include "fakeinstrument.h"

std::unique_ptr<Instrument> createInstrument(const char* name);