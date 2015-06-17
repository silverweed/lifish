#pragma once
/**
 * Entities which can't move (walls, teleporters, powerups, coins, bosses)
 */
#include "Entity.hpp"

namespace Game {

class FixedEntity : public Entity {};

}
