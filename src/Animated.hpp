#pragma once

#include <string>
#include "AnimatedSprite.hpp"

namespace Game {

class Animated {
protected:
	Animation animations[4];
	AnimatedSprite animatedSprite; 
};

}
