#include "utils.hpp"
#include "Component.hpp"
#include "LoopingMusic.hpp"
#include "Options.hpp"
#include "ShadedText.hpp"
#include "json.hpp"
#include <random>
#include <cassert>

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
	auto idx = strnlen(lif::pwd, lif::PWD_BUFSIZE);

	if (idx >= path.length()) return path;

	while (idx < path.length() && path[idx] == lif::DIRSEP)
		++idx;

	return idx < path.length() ? path.substr(idx) : ""; 
}
