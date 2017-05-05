#pragma once

#include "Component.hpp"

namespace lif {

/** Tagging component used for entities which grant life when killed
 *  by a Player with the `absorb` power.
 */
class Absorbable : public lif::Component {
public:
	explicit Absorbable(lif::Entity& owner) : lif::Component(owner) {
		_declComponent<Absorbable>();
	}
};

}
