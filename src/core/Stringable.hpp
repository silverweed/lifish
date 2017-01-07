#pragma once

#include <string>

namespace lif {

class Stringable {
public:
	virtual ~Stringable() {}
	virtual std::string toString() const = 0;
};

}
