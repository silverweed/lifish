#include "language.hpp"
#include "utils.hpp"
#include <iostream>
#include <tinyjson.h>
#include <fstream>
#include <unordered_map>
#include <cassert>

lif::Language lif::curLang = lif::Language::ENGLISH;
bool lif::switchedLanguage = false;

static const char* langMap[static_cast<unsigned>(lif::Language::COUNT)] = {
	"en",
	"it",
	"de",
	"fr"
};
static tinyjson::json l10nStrings;

lif::Language lif::langFromStr(const std::string& name) {
	for (unsigned i = 0; i < static_cast<unsigned>(lif::Language::COUNT); ++i) {
		if (name == langMap[i])
			return static_cast<lif::Language>(i);
	}

	std::cerr << "[ WARNING ] unknown language " << name << "\n";
	return lif::Language::ENGLISH;
}

std::string lif::strFromLang(lif::Language lang) {
	return langMap[static_cast<unsigned>(lang)];
}

void lif::switchLanguage(lif::Language lang) {
	assert(lang < lif::Language::COUNT);
	if (lang != lif::curLang) {
		switchedLanguage = true;
	}
	lif::curLang = lang;
}

bool lif::loadL10nStrings(const std::string& filename) {
	std::cerr << "Loading localization strings from " << filename << "\n";
	try {
		std::string fileContent = lif::readEntireFile(filename);
		l10nStrings = tinyjson::parser::parse(fileContent.c_str());
		return true;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
}

std::string lif::getLocalized(const std::string& strkey) {
	const auto it = l10nStrings.find(strkey);
	if (!it) {
		std::cerr << "[l10n] '" << strkey << "': no such key.\n";
		return "";
	}

	const auto lit = it->find(strFromLang(lif::curLang));
	return lit ? lit->get_string() : "";
}
