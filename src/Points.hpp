#pragma once
/**
 * A temporary sprite showing the points given by something
 */

#include "Temporary.hpp"

namespace Game {

class Points : public Game::Temporary, public Game::ShadedText {
	sf::Vector2f initialPos;
	float speed = 30.f;
public:
	constexpr static unsigned short CHARACTER_SIZE = 10;

	Points(const sf::Vector2f& pos, int amount, sf::Color color = sf::Color::Blue) 
		: Game::Temporary(pos),
		  ShadedText(
			Game::getAsset("fonts", Game::Fonts::POINTS),
			std::to_string(amount),
			pos,
			color,
			sf::Color::Black)
	{
		initialPos = pos;
		setCharacterSize(CHARACTER_SIZE);
	}
	
	void play() override {}
	bool isPlaying() const override {
		return (initialPos - ShadedText::pos).y < 20;
	}

	void setOrigin(const sf::Vector2f& pos) { Game::ShadedText::setOrigin(pos); }

	void draw(sf::RenderTarget& window) override {
		sf::Time frameTime = frameClock.restart();
		ShadedText::pos.y -= speed * frameTime.asSeconds();
		ShadedText::setPosition(ShadedText::pos);
		Game::ShadedText::draw(window);
	}
};

}
