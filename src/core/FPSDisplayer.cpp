#include "FPSDisplayer.hpp"
#include <sstream>

FPSDisplayer::FPSDisplayer(sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Font& font)
	: fpsText(font, "-", pos)
{
	fpsText.setStyle(sf::Text::Style::Bold);
	fpsText.setCharacterSize(20);
}

void FPSDisplayer::update() {
	const auto t = fpsClock.restart().asSeconds();
	curTime += t;
	++nUpdates;
	if (fpsUpdateClock.getElapsedTime().asSeconds() >= 1) {
		const auto fps = static_cast<int>(n_updates / cur_time);
		std::stringstream ss;
		ss << fps << " fps (vsync " << (lif::options.vsync ? "ON" : "OFF") << ")";
		fpsText.setString(ss.str());
		fpsUpdateClock.restart();
		nUpdates = 0;
		curTime = 0;
	}	
}

void FPSDisplayer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	window.draw(fpsText, states);
}
