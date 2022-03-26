#include "filedialogue.h"

std::optional<std::string> fileOpenDialogue(const wchar_t* defFileExt)
{
	ComPtr<IFileOpenDialog> fop;
	hrthrow(CoCreateInstance(
		CLSID_FileOpenDialog,
		nullptr,
		CLSCTX_ALL,
		IID_IFileOpenDialog,
		&fop
	), L"Failed to create file selection dialogue");

	hrthrow(fop->SetDefaultExtension(defFileExt), L"Failed to set default file extension");

	HRESULT hr = fop->Show(nullptr);
	if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		return {};
	hrthrow(hr, L"Failed to show file selection dialogue");

	ComPtr<IShellItem> item;
	hrthrow(fop->GetResult(&item), L"Failed to get result from file selection dialogue");
	
	wchar_t* tmpstr = nullptr;
	hrthrow(item->GetDisplayName(SIGDN_FILESYSPATH, &tmpstr));
	std::unique_ptr<wchar_t, decltype([](wchar_t* ptr){ CoTaskMemFree(ptr); })> str(tmpstr);
	return wstringToString(std::wstring(&*str));
}

std::optional<std::string> fileSaveDialogue(const wchar_t* defFileExt)
{
	ComPtr<IFileSaveDialog> fop;
	hrthrow(CoCreateInstance(
		CLSID_FileSaveDialog,
		nullptr,
		CLSCTX_ALL,
		IID_IFileSaveDialog,
		&fop
	), L"Failed to create file selection dialogue");

	hrthrow(fop->SetDefaultExtension(defFileExt), L"Failed to set default file extension");

	HRESULT hr = fop->Show(nullptr);
	if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		return {};
	hrthrow(hr, L"Failed to show file selection dialogue");

	ComPtr<IShellItem> item;
	hrthrow(fop->GetResult(&item), L"Failed to get result from file selection dialogue");
	
	wchar_t* tmpstr = nullptr;
	hrthrow(item->GetDisplayName(SIGDN_FILESYSPATH, &tmpstr));
	std::unique_ptr<wchar_t, decltype([](wchar_t* ptr){ CoTaskMemFree(ptr); })> str(tmpstr);
	return wstringToString(std::wstring(&*str));
}