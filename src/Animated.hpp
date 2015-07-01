#pragma once

#include <string>
#include "AnimatedSprite.hpp"

namespace Game {

class Animated {
protected:
	static constexpr unsigned short MAX_N_ANIMATIONS = 7;
	Animation animations[MAX_N_ANIMATIONS];
	AnimatedSprite animatedSprite; 
};

}
