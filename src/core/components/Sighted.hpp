#pragma once

#include <utility>
#include <vector>
#include <array>
#include <cstdint>
#include <initializer_list>
#include "collision_layers.hpp"
#include "Component.hpp"

namespace Game {

class EntityGroup;

/** A Sighted entity has knowledge of entities around it.
 *  Use either the specializations AxisSighted or FreeSighted for sight along axes or in all directions.
 *  This component does NOT see killed entities, i.e. entities which have a Killable component with
 *  `isKilled() == true`.
 */
class Sighted : public Game::Component {
protected:
	/** Bitmask for opaque layers (entities with no Collider are always transparent) */
	intmax_t opaqueMask = 0;

	/** The entities which can potentially be seen */
	const Game::EntityGroup *entities = nullptr;

	/** Vision radius in number of tiles. Negative means infinite. */
	float visionRadius;


	bool _isOpaque(Game::Layers::Layer layer) const;

public:
	COMP_NOT_UNIQUE

	explicit Sighted(Game::Entity& owner, float visionRadius);

	/** Sets all layers in `layers` as (`opaque` ? opaque : transparent) */
	void setOpaque(std::initializer_list<Game::Layers::Layer> layers, bool opaque = true);

	void setEntityGroup(const Game::EntityGroup *eg);

	virtual void update() = 0;
};

}
