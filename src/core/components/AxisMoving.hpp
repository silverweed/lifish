#pragma once

#include "Moving.hpp"
#include "Direction.hpp"

namespace lif {

/**
 * An object moving only parallel to axes
 */
class AxisMoving : public lif::Moving {
	lif::Direction direction;
	lif::Direction prevDirection;
	/** Latest tile at which owner was aligned to grid */
	sf::Vector2i prevAlign;
	/** Whether this entity will auto-realign when colliding with a solid entity */
	bool autoRealign = true;
	/** Whether this entity will adjust its align at each tile */
	bool ensureAlign = true;
	/** Whether this entity can switch to opposite direction without being aligned */
	bool fastTurn = false;


	void _ensureAlign();

public:
	explicit AxisMoving(lif::Entity& owner, float speed,
			lif::Direction dir = lif::Direction::NONE);

	lif::Direction getDirection() const { return direction; }
	lif::Direction getPrevDirection() const { return prevDirection; }
	sf::Vector2i getPrevAlign() const { return prevAlign; }

	void setDirection(lif::Direction dir);
	void turn(short straightAngles, bool clockwise);
	void setPrevAlign(const sf::Vector2i& a) { prevAlign = a; }
	/** If set to `false`, entity won't auto-realign when colliding with solid OR when stop()ped.
	 *  This is useful for AI-managed moving entities. Default: true
	 */
	void setAutoRealign(bool b) { autoRealign = b; }
	bool isAutoRealignEnabled() const { return autoRealign; }
	/** If set to `false`, entity won't guarantee to always be aligned to the tile
	 *  it's passing through for at least 1 frame. This should never be true
	 *  for entities which check for their alignment to accomplish some logic.
	 *  Useful for bullets.
	 */
	void setEnsureAlign(bool b) { ensureAlign = b; }
	bool isEnsureRealignEnabled() const { return ensureAlign; }

	void setFastTurn(bool b) { fastTurn = b; }

	void realign();
	void stop() override;
	void update() override;
};

}
