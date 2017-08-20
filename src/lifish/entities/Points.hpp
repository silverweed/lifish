#pragma once

#include "Entity.hpp"
#include "ShadedText.hpp"

namespace lif {

/**
 * A temporary sprite showing a small text rising from a position
 */
class Points : public lif::Entity {
	const sf::Vector2f initialPos;
	lif::ShadedText text;

public:
	constexpr static unsigned short CHARACTER_SIZE = 10;

	explicit Points(const sf::Vector2f& pos, const std::string& str,
		sf::Color color = sf::Color::Blue,
		unsigned short charSize = CHARACTER_SIZE);

	explicit Points(const sf::Vector2f& pos, int pts,
		sf::Color color = sf::Color::Blue,
		unsigned short charSize = CHARACTER_SIZE);

	void update() override;
	void setOrigin(const sf::Vector2f& pos) override;

	sf::FloatRect getGlobalBounds() const;
};

}
