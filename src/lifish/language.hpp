#pragma once

#include <string>

namespace lif {

enum class Language {
	ENGLISH,
	ITALIAN,
	GERMAN,
	FRENCH,
	COUNT
};

constexpr auto LOCALIZATION_STRINGS_FILE = "strings.json";

extern Language curLang;
extern bool switchedLanguage;

void switchLanguage(Language lang);
bool loadL10nStrings(const std::string& fileName);
std::string getLocalized(const std::string& strkey);

lif::Language langFromStr(const std::string& name);
std::string strFromLang(lif::Language lang);

}
