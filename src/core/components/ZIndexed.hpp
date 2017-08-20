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
		_declComponent<ZIndexed>();
	}

	virtual int getZIndex() const { return zIndex; }
	virtual void setZIndex(int z) { zIndex = z; }
};

}
