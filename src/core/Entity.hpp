#pragma once

#include <memory>
#include <vector>
#include <SFML/System.hpp>
#include "WithOrigin.hpp"
#include "Stringable.hpp"

namespace Game {

class Component;

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity : public Game::WithOrigin, public Game::Stringable {
protected:
	std::vector<std::unique_ptr<Game::Component>> components;
	sf::Vector2f position;
	std::string _toString(unsigned short indent) const;

public:
	explicit Entity();

	explicit Entity(const sf::Vector2f& pos);
	virtual ~Entity();

	template<class T>
	T* addComponent(T* comp);

	template<class T>
	T* get() const;

	const sf::Vector2f& getPosition() const; 
	virtual void setPosition(const sf::Vector2f& p); 

	bool isAligned(const char axis = 'b') const;

	virtual void update();

	/** Implements WithOrigin */
	virtual void setOrigin(const sf::Vector2f& origin) override;
	
	/** Implements Stringable */
	virtual std::string toString() const override;
};


/// Implementation

template<class T>
T* Entity::addComponent(T* comp) {
	if (T::requiredUnique() && get<T>() != nullptr)
		throw std::logic_error("Two components of the same type were added to this Entity!");
	components.push_back(std::unique_ptr<Game::Component>(comp));
	return comp;
} 

template<class T>
T* Entity::get() const {
	for (auto& comp : components) {
		Component *ptr = comp.get();
		T* derived = nullptr;
		if (ptr && (derived = dynamic_cast<T*>(ptr)))
			return derived;
	}
	return nullptr;
}

}
