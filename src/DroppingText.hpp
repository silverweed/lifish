#pragma once

#include "Temporary.hpp"

namespace Game {

/**
 * A temporary sprite dropping down from above.
 * Used for "Hurry Up!" and "GAME OVER" texts.
 */
class DroppingText : public Game::Temporary, public sf::Drawable {
	float speed;
	sftools::Chronometer pauseClock;
	bool stoppedAtMiddle = false,
	     playing = false;

public:
	DroppingText(const std::string& texture_name, 
			const sf::Vector2i& texture_rect, float speed = 230.f);

	/** Resets all internal parameters and sets playing = true. 
	 *  playing is automatically set to false by move() when the
	 *  position of this text exits the screen.
	 */
	void play() override;

	bool isPlaying() const override {
		return playing;
	}

	void move();

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override {
		window.draw(animatedSprite, states);
	}

	void draw(sf::RenderTarget& window) override {
		window.draw(animatedSprite);
	}
};

}
