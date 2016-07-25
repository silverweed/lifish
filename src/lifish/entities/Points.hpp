#pragma once

#include "Entity.hpp"
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

	explicit Points(const sf::Vector2f& pos, int pts, 
		sf::Color color = sf::Color::Blue, 
		unsigned short charSize = CHARACTER_SIZE);

	void update() override;
	void setOrigin(const sf::Vector2f& pos) override {
		Game::Entity::setOrigin(pos);
		text.setOrigin(pos);
	}
};

}
