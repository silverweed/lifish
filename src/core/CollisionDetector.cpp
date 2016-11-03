#include "CollisionDetector.hpp"
#include "EntityGroup.hpp"

using Game::CollisionDetector;

CollisionDetector::CollisionDetector(Game::EntityGroup& group)
	: group(group) 
{}
