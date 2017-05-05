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

void lif::maybeShowFPS(sf::RenderWindow& window, const sf::Vector2f& pos) {
	static double cur_time;
	static int n_updates = 0;
	static sf::Clock fps_clock, fps_update_clock;
	static lif::ShadedText fps_text(lif::getAsset("fonts", lif::fonts::DEBUG_INFO), "-", pos);
	static bool textSetUp = false;

	// Setup text once
	if (!textSetUp) {
		fps_text.setStyle(sf::Text::Style::Bold);
		fps_text.setCharacterSize(20);
		textSetUp = true;
	}	

	float t = fps_clock.restart().asSeconds();
	if (lif::options.showFPS) {
		cur_time += t;
		++n_updates;
		if (fps_update_clock.getElapsedTime().asSeconds() >= 1) {
			int fps = static_cast<int>(n_updates / cur_time);
			std::stringstream ss;
			ss << fps << " fps (vsync " << (lif::options.vsync ? "ON" : "OFF") << ")";
			fps_text.setString(ss.str());
			fps_update_clock.restart();
			n_updates = 0;
			cur_time = 0.;
		}	
		window.draw(fps_text);
	}
}

void lif::testMusic() {
	sf::Music sample;
	sample.openFromFile(lif::getAsset("music", "music1.ogg"));
	sample.play();
	sample.setVolume(lif::options.musicVolume);
	sf::sleep(sf::milliseconds(200));
	sample.stop();
}
