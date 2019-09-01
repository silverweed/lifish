#include "FPSDisplayer.hpp"
#include "Options.hpp"
#include "core.hpp"
#include <sstream>

using lif::FPSDisplayer;

FPSDisplayer::FPSDisplayer(const sf::Vector2f& pos, const std::string& fontname)
	: fpsText(fontname, "-", pos)
{
	fpsText.setStyle(sf::Text::Style::Bold);
	fpsText.setCharacterSize(16);
	fpsText.setColor(sf::Color(0xDDDDDDEE), sf::Color(0x222222EE));
	fpsText.setShadowSpacing(1, 1);
}

void FPSDisplayer::update(const sf::Vector2u& windowBounds) {
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

	const auto bounds = fpsText.getGlobalBounds();
	fpsText.setPosition(sf::Vector2f(windowBounds.x - bounds.width - 5, windowBounds.y - bounds.height - 3));
}

void FPSDisplayer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(fpsText, states);
}
