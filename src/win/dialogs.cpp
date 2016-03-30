#include "dialogs.hpp"
#include <wchar.h>
#include <iostream>

static std::string _showDialog(bool open) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
					
	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Couldn't initialize COM" << std::endl;
		return "";
	}

	IFileDialog *fileDialog;

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(
		open ? CLSID_FileOpenDialog : CLSID_FileSaveDialog, NULL, CLSCTX_ALL, 
		open ? IID_IFileOpenDialog : IID_IFileSaveDialog,
		reinterpret_cast<void**>(&fileDialog));

	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Couldn't create FileDialog instance" << std::endl;
		return "";
	}
	
	// TODO: set default folder = pwd
	_COMDLG_FILTERSPEC filters[] = {
		{ L"Lifish save file", L"*.lifish" }, 
		{ L"All files", L"*.*" }
	};
	fileDialog->SetFileTypes(2, filters); 
	fileDialog->SetDefaultExtension(L"lifish");
	hr = fileDialog->Show(NULL);

	if (!SUCCEEDED(hr)) {
		return "";
	}

	IShellItem *item;
	hr = fileDialog->GetResult(&item);

	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Failed to get result from dialog." << std::endl;
		return "";
	}

	PWSTR filePath;
	hr = item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	
	if (!SUCCEEDED(hr)) {
		std::cerr << "[ ERROR ] Failed to get file display name." << std::endl;
		return "";
	}

	char str[2048];
	str[2047] = '\0';
	wcstombs(str, filePath, 2047);

	std::string path(str);

	item->Release();
	fileDialog->Release();
	CoUninitialize();

	return path;
}

std::string Game::Dialog::openFile() {
	return _showDialog(true);
}

std::string Game::Dialog::saveFile() { 
	return _showDialog(false);
}
