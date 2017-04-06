#include "SaveDataBrowser.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <dirent.h>
#include <cstdlib>
#include <cstring>

using lif::SaveDataBrowser;

SaveDataBrowser::SaveDataBrowser() : idnum(0) {}

std::vector<SaveDataBrowser::SaveFile> SaveDataBrowser::browseSaveData(std::string path) const {
	std::vector<SaveDataBrowser::SaveFile> files;

	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		return files;
	
	auto ent = readdir(dir);
	while (ent != NULL) {
		char *suffix = strrchr(ent->d_name, '.');
		if (suffix != NULL && strcmp(suffix, ".lifish") == 0) {
			SaveDataBrowser::SaveFile file;
			file.displayName = std::string(ent->d_name, strlen(ent->d_name) - strlen(suffix));
			file.path = path + lif::DIRSEP + ent->d_name;
			file.level = 0; // TODO
			files.emplace_back(file);
		}
		ent = readdir(dir);
	}
	closedir(dir);

	return files;
}

std::string SaveDataBrowser::_newUniqueId() {
	return lif::to_string(idnum++);
}

lif::ui::Screen::Callback SaveDataBrowser::_createDeleteCallback(const std::string& path) const {
	return [path] () {
		// TODO: ask for confirmation
		if (remove(path.c_str()) != 0) {
			perror("Error deleting file");
		}
		return lif::ui::Action::DO_NOTHING;
	};
}

lif::ui::Screen::Callback SaveDataBrowser::_createLoadCallback(const std::string& path) {
	return [this, path] () {
		loadedData = lif::SaveManager::loadGame(path);
		return lif::ui::Action::LOAD_GAME;
	};
}
