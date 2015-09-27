#include "dialogs.hpp"
#include <iostream>

std::wstring Game::Dialog::openFile() {
	std::wstring path;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
					
	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Couldn't initialize COM" << std::endl;
		return path;
	}

	IFileOpenDialog *pFileOpen;

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Couldn't create FileOpenDialog instance" << std::endl;
		return path;
	}

	hr = pFileOpen->Show(NULL);

	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Failed to show file dialog." << std::endl;
		return path;
	}

	IShellItem *pItem;
	hr = pFileOpen->GetResult(&pItem);

	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Failed to get result from dialog." << std::endl;
		return path;
	}

	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

	path = std::wstring(pszFilePath);

	pItem->Release();
	pFileOpen->Release();
	CoUninitialize();

	return path;
}

std::string Game::Dialog::saveFile() { return ""; }
