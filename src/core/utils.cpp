#include "utils.hpp"
#include "Component.hpp"
#include "LoopingMusic.hpp"
#include "Options.hpp"
#include "ShadedText.hpp"
#include "json.hpp"
#include <cassert>
#include <random>
#if defined(_WIN32) || defined(__MINGW32__)
#	include <windows.h>
#else
#	include <sys/stat.h>
#endif

using json = nlohmann::json;

void sf::to_json(json& j, const sf::Time& time) {
	j = time.asMilliseconds();
}

void sf::from_json(const json& j, sf::Time& time) {
	time = sf::milliseconds(j);
}

void sf::to_json(json& j, const sf::VideoMode& vm) {
	j = { vm.width, vm.height, vm.bitsPerPixel };
}

void sf::from_json(const json& j, sf::VideoMode& vm) {
	vm.width = j[0];
	vm.height = j[1];
	vm.bitsPerPixel = j[2];
}

void lif::testMusic() {
	sf::Music sample;
	sample.openFromFile(lif::getAsset("music", "music1.ogg"));
	sample.play();
	sample.setVolume(lif::options.musicVolume);
	sf::sleep(sf::milliseconds(200));
	sample.stop();
}

std::string lif::toRelativePath(const std::string& path) {
	auto idx = lif::assetDir.length();

	if (idx >= path.length()) return path;

	while (idx < path.length() && path[idx] == lif::DIRSEP)
		++idx;

	return idx < path.length() ? path.substr(idx) : "";
}

sf::View lif::keepRatio(const sf::Vector2f& size, const sf::Vector2u& designedsize) {
	sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

	const float screenwidth = size.x / static_cast<float>(designedsize.x),
	            screenheight = size.y / static_cast<float>(designedsize.y);

	if (screenwidth > screenheight) {
		viewport.width = screenheight / screenwidth;
		viewport.left = (1.f - viewport.width) / 2.f;
	} else if (screenwidth < screenheight) {
		viewport.height = screenwidth / screenheight;
		viewport.top = (1.f - viewport.height) / 2.f;
	}

	sf::View view(sf::FloatRect(0, 0, designedsize.x , designedsize.y));
	view.setViewport(viewport);

	return view;
}

bool lif::createDirIfNotExisting(const std::string& path) {
#if defined(_WIN32) || defined(__MINGW32__)
	bool ok = !!CreateDirectory(path.c_str(), NULL);
	return ok || GetLastError() == ERROR_ALREADY_EXISTS;
#else
	bool ok = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
	return ok || errno == EEXIST;
#endif
}
