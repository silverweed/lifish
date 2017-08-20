#include "FPSDisplayer.hpp"
#include "core.hpp"
#include "Options.hpp"
#include <sstream>

using lif::FPSDisplayer;

FPSDisplayer::FPSDisplayer(const sf::Vector2f& pos, const std::string& fontname)
	: fpsText(fontname, "-", pos)
{
	fpsText.setStyle(sf::Text::Style::Bold);
	fpsText.setCharacterSize(20);
}

void FPSDisplayer::update() {
	const auto t = fpsClock.restart().asSeconds();
	curTime += t;
	++nUpdates;
	if (fpsUpdateClock.getElapsedTime().asSeconds() >= 1) {
		const auto fps = static_cast<int>(nUpdates / curTime);
		std::stringstream ss;
		ss << fps << " fps (vsync " << (lif::options.vsync ? "ON" : "OFF") << ")";
		fpsText.setString(ss.str());
		fpsUpdateClock.restart();
		nUpdates = 0;
		curTime = 0;
	}
}

void FPSDisplayer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(fpsText, states);
}
