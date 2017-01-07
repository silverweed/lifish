#pragma once

#include "Component.hpp"

namespace lif {

/**
 * A Fixed Entity is an entity which cannot ever move.
 * This enables some optimizations on this Component's owner, particularly for its lookup in
 * an EntityGroup (i.e. a tile can be checked for its presence in a random access fashion).
 */
class Fixed : public lif::Component {
public:
	explicit Fixed(lif::Entity& owner)
		: lif::Component(owner)
	{}
};

}
