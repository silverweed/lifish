#pragma once

#include <string>

namespace Game {

class Stringable {
public:
	virtual ~Stringable() {}
	virtual std::string toString() const = 0;
};

}
