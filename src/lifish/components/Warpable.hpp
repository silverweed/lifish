#pragma once

#include "Component.hpp"

namespace lif {

/** Entities that can use Teleports */
class Warpable : public lif::Component {
	int _justWarped = 0;

public:
	explicit Warpable(lif::Entity& owner) : lif::Component(owner) {
		_declComponent<Warpable>();
	}

	void update() override {
		lif::Component::update();
		if (_justWarped > 0) --_justWarped;
	}

	bool justWarped() const { return _justWarped > 0; }
	void setJustWarped(bool w) { _justWarped = 2 * w; }
};

}
