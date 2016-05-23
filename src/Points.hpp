#pragma once

#include "Entity.hpp"
#include "Temporary.hpp"
#include "ShadedText.hpp"

namespace Game {

/**
 * A temporary sprite showing a small text rising from a position
 */
class Points : public Game::Entity {
	const sf::Vector2f initialPos;
	Game::ShadedText text;

public:
	constexpr static unsigned short CHARACTER_SIZE = 10;
	constexpr static float SPEED = 30.f;

	explicit Points(const sf::Vector2f& pos, const std::string& str, 
		sf::Color color = sf::Color::Blue, 
		unsigned short charSize = CHARACTER_SIZE);

	void update() override;
	void draw(sf::RenderTarget& window) override;

	using Game::ShadedText::setPosition;
	using Game::ShadedText::getPosition;
	using Game::ShadedText::getGlobalBounds;
	using Game::ShadedText::setOrigin;
};

}
