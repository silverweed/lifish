#pragma once

#include "Temporary.hpp"
#include "ShadedText.hpp"

namespace Game {

/**
 * A temporary sprite showing a small text rising from a position
 */
class Points : public Game::Temporary, public Game::ShadedText {
	const sf::Vector2f initialPos;
	float speed = 30.f;

public:
	constexpr static unsigned short CHARACTER_SIZE = 10;

	Points(const sf::Vector2f& pos, const std::string& str, 
		sf::Color color = sf::Color::Blue, 
		unsigned short charSize = CHARACTER_SIZE);

	void play() override {}
	bool isPlaying() const override {
		return (initialPos - ShadedText::pos).y < 20;
	}

	sf::Vector2f getPosition() const {
		return Game::ShadedText::getPosition();
	}
	void setPosition(const sf::Vector2f& pos) override { 
		Game::ShadedText::setPosition(pos); 
	}

	sf::FloatRect getGlobalBounds() const override { 
		return Game::ShadedText::getGlobalBounds(); 
	}

	void draw(sf::RenderTarget& window) override;

	using Game::ShadedText::setOrigin;
};

}
