#pragma once

#include "Component.hpp"

namespace lif {

/**
 * A Fixed Entity is an entity which cannot ever move.
 * This enables some optimizations on this Component's owner, particularly for collision detection.
 * Note that failing to add a Fixed component to an immovable Entity is likely to cause
 * errors in enemies' AI and other collision-related issues.
 */
class Fixed : public lif::Component {
public:
	explicit Fixed(lif::Entity& owner)
		: lif::Component(owner)
	{
		_declComponent<Fixed>();
	}
};

}
