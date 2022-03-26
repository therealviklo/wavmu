#pragma once
#include <optional>
#include "win.h"
#include "winerror.h"
#include <shobjidl.h>
#include "utils.h"

std::optional<std::string> fileOpenDialogue(const wchar_t* defFileExt);
std::optional<std::string> fileSaveDialogue(const wchar_t* defFileExt);