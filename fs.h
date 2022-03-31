#pragma once
#include <filesystem>
#include "utils.h"
#include "winerror.h"

namespace fs = std::filesystem;

fs::path getExecutableFolder();
inline fs::path getInstrumentsFolder() { return getExecutableFolder() / "instruments"; }