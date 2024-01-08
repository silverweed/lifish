#pragma once

#include <string>
#include <vector>
#include "SaveManager.hpp"
#include "Screen.hpp"

namespace lif {

class SaveDataBrowser {
	long idnum = 0;
	lif::SaveData loadedData;

protected:
	struct SaveFile {
		std::string displayName;
		std::string path;
		std::time_t mtime;
		int level = 0;
		int nPlayers = 0;

		bool operator <(const SaveFile& sf) const {
			return std::difftime(mtime, sf.mtime) < 0;
		}
	};

	SaveDataBrowser();

	lif::ui::Screen::Callback _createLoadCallback(const std::string& file);
	lif::ui::Screen::Callback _createDeleteCallback(const std::string& file) const;
	std::string _newUniqueId();

public:
	std::vector<SaveFile> browseSaveData(std::string saveDataPath) const;
	const lif::SaveData& getLoadedData() const { return loadedData; }
};

}
