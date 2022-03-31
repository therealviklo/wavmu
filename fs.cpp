#include "fs.h"

fs::path getExecutableFolder()
{
	wchar_t path[MAX_PATH];
	if (!GetModuleFileNameW(nullptr, path, MAX_PATH))
		throw WinError(L"Failed to get executable file name");
	return fs::path(path).parent_path();
}