#pragma once

#include <memory>
#include <list>
#include <SFML/System.hpp>
#include "WithOrigin.hpp"

namespace Game {

class Component;

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity : public Game::WithOrigin {
	std::list<std::unique_ptr<Game::Component>> components;

protected:
	sf::Vector2f position;

public:
	Entity() {}
	Entity(const sf::Vector2f& pos) : position(pos) {}
	virtual ~Entity() {}

	template<class T>
	T* addComponent(T* comp) {
		components.push_back(std::unique_ptr<Game::Component>(comp));
		return comp;
	}

	template<class T>
	T* get() const {
		for (auto& comp : components) {
			Component *ptr = comp.get();
			if (T* derived = dynamic_cast<T*>(ptr))
				return derived;
		}
		return nullptr;
	}

	const sf::Vector2f& getPosition() const { return position; }
	void setPosition(const sf::Vector2f& p) { position = p; }

	virtual void update();

	virtual void setOrigin(const sf::Vector2f& origin) override;

	bool isAligned(const char axis = 'b') const;
};

}
