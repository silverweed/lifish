#pragma once

#include "Component.hpp"

namespace lif {

/**
 * This is a tagging Component with no functionality.
 * The current level is considered "clear" when there are no "Foe" Entities left.
 */
class Foe : public lif::Component {
public:
	explicit Foe(lif::Entity& owner) : lif::Component(owner) {
		_declComponent<Foe>();
	}
};

}
