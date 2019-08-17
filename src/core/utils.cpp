#include "utils.hpp"
#include "LoopingMusic.hpp"
#include "Options.hpp"
#include "ShadedText.hpp"
#include "json.hpp"
#include <random>

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
