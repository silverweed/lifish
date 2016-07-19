#pragma once

/** Provides glue code for dialogs, using WINAPI on Windows
 *  and NFD wherever possible.
 */
#include <iostream>
#include <string>
#include "core.hpp"
#ifdef SFML_SYSTEM_WINDOWS
#	include "win/dialogs.hpp"
#else
#	include "nfd.h"
#endif

namespace Game {

/** Displays a file browser to locate a lifish save file. Returns either the file name or ""
 *  if the loading was canceled / had errors.
 */
std::string display_load_dialog() {
#if defined(SFML_SYSTEM_WINDOWS)
	return Game::Dialog::openFile();
#elif !defined(HAVE_NFD)
	std::cerr << "[ WARNING ] lifish was compiled without GTK support:\n"
		     "if you want to load a game, place a file named `save.lifish`\n"
		     "in the directory where the lifish executable resides."
		  << std::endl;
	return "save.lifish";
#else
	nfdchar_t *outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("lifish", Game::pwd, &outPath);
	switch (result) {
	case NFD_OKAY:
		{
			std::string path(outPath);
			free(outPath);
			return path;
		}
	case NFD_ERROR:
		std::cerr << "Error opening file: " << NFD_GetError() << std::endl;
		// fallthrough
	default:
		return "";
	}
#endif
}

/** Displays a file browser to create a new lifish save file. Returns either the file name or ""
 *  if the loading was canceled / had errors.
 */
std::string display_save_dialog() {
#if defined(SFML_SYSTEM_WINDOWS)
	return Game::Dialog::saveFile();
#elif !defined(HAVE_NFD)
	std::cerr << "[ WARNING ] lifish was compiled without GTK support:\n"
		     "the game will be saved in `" << Game::pwd << Game::DIRSEP << "save.lifish`."
		  << std::endl;
	return "save.lifish";
#else
	nfdchar_t *outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog("lifish", Game::pwd, &outPath);
	switch (result) {
	case NFD_OKAY:
		{
			std::string path(outPath);
			free(outPath);
			return path;
		}
	case NFD_ERROR:
		std::cerr << "Error opening file: " << NFD_GetError() << std::endl;
		// fallthrough
	default:
		return "";
	}
#endif
}

}
