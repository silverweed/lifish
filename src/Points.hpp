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

	void draw(sf::RenderTarget& window) override;

	using Game::ShadedText::setPosition;
	using Game::ShadedText::getPosition;
	using Game::ShadedText::getGlobalBounds;
	using Game::ShadedText::setOrigin;
};

}
