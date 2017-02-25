#pragma once

#include <string>
#include <vector>
#include "Screen.hpp"

namespace lif {

class SaveDataBrowser {
	long idnum = 0;

protected:
	struct SaveFile {
		std::string displayName;
		std::string path;
		int level;
	};

	SaveDataBrowser();

	lif::ui::Screen::Callback _createLoadCallback(const std::string& file) const;
	lif::ui::Screen::Callback _createDeleteCallback(const std::string& file) const;
	std::string _newUniqueId();

	std::vector<SaveFile> browseSaveData(std::string saveDataPath) const;
};

}
