#pragma once

#include <memory>
#include <list>
#include <SFML/System.hpp>
#include "WithOrigin.hpp"
#include "Stringable.hpp"

namespace Game {

class Component;

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity : public Game::WithOrigin, public Game::Stringable {
	std::list<std::unique_ptr<Game::Component>> components;

protected:
	sf::Vector2f position;
	std::string _toString(unsigned short indent) const;

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
			T* derived = nullptr;
			if (ptr && (derived = dynamic_cast<T*>(ptr)))
				return derived;
		}
		return nullptr;
	}

	virtual const sf::Vector2f& getPosition() const { return position; }
	virtual void setPosition(const sf::Vector2f& p) { position = p; }

	bool isAligned(const char axis = 'b') const;

	virtual void update();

	/** Implements WithOrigin */
	virtual void setOrigin(const sf::Vector2f& origin) override;
	
	/** Implements Stringable */
	virtual std::string toString() const override {
		return _toString(0);
	}
};

}
