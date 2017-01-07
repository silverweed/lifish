#include "Component.hpp"

using lif::Component;

Component::Component(lif::Entity& owner)
	: owner(owner)
{}
