#pragma once

#include "Temporary.hpp"

namespace Game {

/**
 * A temporary sprite dropping down from above.
 * Used for "Hurry Up!" and "GAME OVER" texts.
 */
class DroppingText : public Game::Temporary {
	float speed;
	sftools::Chronometer pauseClock;
	bool stopped = false;

public:
	DroppingText(const std::string& texture_name, 
			const sf::Vector2i& texture_rect, float speed = 230.f);

	void play() override {}
	bool isPlaying() const override {
		return pos.y < (Game::LEVEL_HEIGHT + 2) * Game::TILE_SIZE;
	}

	void draw(sf::RenderTarget& window) override;
};

}
