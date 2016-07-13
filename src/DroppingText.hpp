#pragma once

#include "Entity.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"

namespace Game {

/**
 * A temporary sprite dropping down from above.
 * Used for "Hurry Up!" and "GAME OVER" texts.
 */
class DroppingText : public Game::Entity {
	const sf::Time PAUSE_TIME = sf::seconds(1);
	Game::Clock *pauseClock = nullptr;
	Game::AxisMoving *moving = nullptr;
	float height = 0;
	bool stoppedAtMiddle = false,
	     playing = false;

public:
	explicit DroppingText(const std::string& texture_name, 
			const sf::Vector2i& texture_rect, float speed = 230.f);

	/** Resets all internal parameters and sets playing = true. 
	 *  playing is automatically set to false by move() when the
	 *  position of this text exits the screen.
	 */
	void play();
	void update() override;
};

}
