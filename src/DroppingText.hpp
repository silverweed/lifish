#pragma once
/**
 * A temporary sprite dropping down from above.
 * Used for "Hurry Up!" and "GAME OVER" texts.
 */

#include "Temporary.hpp"

namespace Game {

class DroppingText : public Game::Temporary {
	float speed;
	sf::Clock pauseClock;
	bool stopped = false;
public:
	DroppingText(const std::string& texture_name, const sf::Vector2i& texture_rect, float speed = 200.f);

	void play() override {}
	bool isPlaying() const override {
		return pos.y < (Game::LEVEL_HEIGHT + 2) * Game::TILE_SIZE;
	}

	void setOrigin(const sf::Vector2f& pos) override { sprite.setOrigin(pos); }

	void draw(sf::RenderTarget& window) override;
};

}
