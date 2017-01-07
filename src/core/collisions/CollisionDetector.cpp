#include "CollisionDetector.hpp"
#include "EntityGroup.hpp"

using Game::CollisionDetector;

CollisionDetector::CollisionDetector(Game::EntityGroup& group, const sf::FloatRect& limit)
	: group(group)
	, levelLimit(limit)
{}
