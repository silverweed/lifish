#pragma once

#include "Moving.hpp"
#include "Direction.hpp"

namespace Game {

/**
 * An object moving only parallel to axes 
 */
class AxisMoving : public Game::Moving {
	Game::Direction direction;
	Game::Direction prevDirection;
	sf::Vector2i prevAlign;
	/** Whether this entity will auto-realign when colliding with a solid entity */
	bool autoRealign = true;
	/** Whether this entity will adjust its align at each tile */
	bool ensureAlign = true;


	void _ensureAlign();

public:
	explicit AxisMoving(Game::Entity& owner, float speed, 
			Game::Direction dir = Game::Direction::NONE);

	Game::Direction getDirection() const { return direction; }
	Game::Direction getPrevDirection() const { return prevDirection; }
	sf::Vector2i getPrevAlign() const { return prevAlign; }

	void setDirection(Game::Direction dir);
	void turn(short straightAngles, bool clockwise);
	void setPrevAlign(const sf::Vector2i& a) { prevAlign = a; }
	/** If set to `false`, entity won't auto-realign when colliding with solid OR when stop()ped.
	 *  This is useful for AI-managed moving entities. Default: true
	 */
	void setAutoRealignEnabled(bool b) { autoRealign = b; }
	bool isAutoRealignEnabled() const { return autoRealign; }
	/** If set to `false`, entity won't guarantee to always be aligned to the tile
	 *  it's passing through for at least 1 frame. This should never be true
	 *  for entities which check for their alignment to accomplish some logic.
	 *  Useful for bullets.
	 */
	void setEnsureAlignEnabled(bool b) { ensureAlign = b; }
	bool isEnsureRealignEnabled() const { return ensureAlign; }

	void realign();
	void stop() override;
	void update() override;
};

}
