#include "utils.hpp"
#include "Options.hpp"
#include "LoopingMusic.hpp"
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

void lif::testMusic() {
	sf::Music sample;
	sample.openFromFile(lif::getAsset("music", "music1.ogg"));
	sample.play();
	sample.setVolume(lif::options.musicVolume);
	sf::sleep(sf::milliseconds(200));
	sample.stop();
}
