#pragma once

#include "Component.hpp"

namespace lif {

class ZIndexed : public lif::Component {
	int zIndex = 0;
public:
	explicit ZIndexed(lif::Entity& owner, int z) 
		: lif::Component(owner)
		, zIndex(z) 
	{
		keys.emplace_back(_getKey<ZIndexed>());
	}

	virtual int getZIndex() const { return zIndex; }
	virtual void setZIndex(int z) { zIndex = z; }
};

}
