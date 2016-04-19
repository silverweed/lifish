#pragma once

#include <string>

namespace Game {

class Stringable {
public:
	virtual std::string toString() const = 0;
};

}
