#pragma once

#include <string>
#include "AnimatedSprite.hpp"

namespace Game {

class Animated {
protected:
	std::map<std::string, Animation> animations;
	AnimatedSprite animatedSprite; 
};

}
