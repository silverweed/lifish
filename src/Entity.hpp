#pragma once

#include <memory>
#include <list>

namespace Game {

class Component;

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity {
	std::list<std::unique_ptr<Game::Component>> components;

public:
	Entity() {}

	template <class T>
	T* addComponent(T* comp);

	template <class T>
	T* get() const;
};

}
