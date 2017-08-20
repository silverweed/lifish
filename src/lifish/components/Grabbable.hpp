#pragma once

#include "Component.hpp"

namespace lif {

class Grabbable : public lif::Component {
	bool grabbed = false;
	lif::Entity *grabbingEntity = nullptr;

public:
	explicit Grabbable(lif::Entity& owner)
		: lif::Component(owner)
	{
		_declComponent<Grabbable>();
	}

	/** `grab` shall be called when the Grabbable is grabbed to avoid reprocess it */
	void grab() { grabbed = true; }
	bool isGrabbed() const { return grabbed; }

	void setGrabbingEntity(lif::Entity *e) { grabbingEntity = e; }
	lif::Entity* getGrabbingEntity() const { return grabbingEntity; }
};

}
