#include "Component.hpp"

using Game::Component;

Component::Component(Game::Entity& owner)
	: owner(owner)
{}
