#pragma once

#include "Component.hpp"

namespace Game {

class ZIndexed : public Game::Component {
	int zIndex = 0;
public:
	explicit ZIndexed(Game::Entity& owner, int z) 
		: Game::Component(owner)
		, zIndex(z) {}

	virtual int getZIndex() const { return zIndex; }
	virtual void setZIndex(int z) { zIndex = z; }
};

}
