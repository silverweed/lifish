#pragma once

#include "Component.hpp"

namespace Game {

class Grabbable : public Game::Component {
	bool grabbed = false;
	Game::Entity *grabbingEntity = nullptr;

public:
	explicit Grabbable(Game::Entity& owner)
		: Game::Component(owner) 
	{}

	/** `grab` shall be called when the Grabbable is grabbed to avoid reprocess it */
	void grab() { grabbed = true; }
	bool isGrabbed() const { return grabbed; }

	void setGrabbingEntity(Game::Entity *e) { grabbingEntity = e; }
	Game::Entity* getGrabbingEntity() const { return grabbingEntity; }
};

}
