#pragma once

#include "Entity.hpp"

namespace lif {

class AxisMoving;

/**
 * A sprite dropping down from above, used for "Hurry Up!" text and similar.
 * A DroppingSprite is created by default over the top of the game viewport,
 * and stays there (hidden) until play() is called.
 * The DroppingSprite will auto-reset as soon as it goes out of the bottom of the screen,
 * so it can be play()ed again.
 */
class DroppingSprite : public lif::Entity {
	const sf::Time PAUSE_TIME = sf::seconds(1);
	const sf::Vector2f origPosition;
	const float height;

	sf::Time pauseT;
	lif::AxisMoving *moving = nullptr;
	bool stoppedAtMiddle = false;
	bool playing = false;

public:
	explicit DroppingSprite(const std::string& texture_name,
			const sf::Vector2i& texture_rect, float speed = 230.f);

	/** Makes the DroppingSprite start moving down. */
	void play();
	/** Stops the DroppingSprite and resets its position */
	void reset();
	void update() override;

	bool isPlaying() const { return playing; }
};

}
