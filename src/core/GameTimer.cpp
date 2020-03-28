#include "GameTimer.hpp"
#include "Time.hpp"
#include "game.hpp"
#include <string>
#include <cstring>

using lif::GameTimer;

GameTimer::GameTimer(const std::string& fontname)
	: text(fontname, "", sf::Vector2f(lif::SIDE_PANEL_WIDTH + 5., lif::GAME_HEIGHT - 20))
{
	text.setCharacterSize(14);
	text.setShadowSpacing(1, 1);
	text.setFGColor(sf::Color::Green);
	bg.setFillColor(sf::Color::Black);
	bg.setSize(sf::Vector2f(90, 21));
	bg.setPosition(text.getPosition() - sf::Vector2f(3.f, 3.f));
}

void GameTimer::update() {
	time += lif::time.getDelta();
	const auto totSeconds = time.asSeconds();
	const auto hours = static_cast<int>(totSeconds / 3600);
	const auto minutes = static_cast<int>(totSeconds - hours * 3600) / 60;
	const auto seconds = static_cast<int>(totSeconds - hours * 3600 - minutes * 60);
	const auto milliseconds = (time.asMilliseconds() - seconds * 1000 - minutes * 60'000 - hours * 3'600'000) / 100;
	char buffer[32] = {0};
	sprintf(buffer, "%.2d:%.2d:%.2d.%.1d", hours, minutes, seconds, milliseconds);
	text.setString(buffer);
}

void GameTimer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bg, states);
	target.draw(text, states);
}
