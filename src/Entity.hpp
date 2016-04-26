#pragma once

#include <memory>
#include <list>
#include <SFML/System.hpp>

namespace Game {

class Component;

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity {
	std::list<std::unique_ptr<Game::Component>> components;

protected:
	sf::Vector2f position;

public:
	Entity() {}
	Entity(const sf::Vector2f& pos) : position(pos) {}

	template<class T>
	T* addComponent(T* comp);

	template<class T>
	T* get() const;

	const sf::Vector2f& getPosition() const { return position; }
	void setPosition(const sf::Vector2f& p) { position = p; }

	virtual void update() {}
};

}
