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
	bool _initialized = false;

protected:
	std::vector<std::shared_ptr<Game::Component>> components;
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

	template<class T>
	std::vector<T*> getAll() const;

	template<class T>
	std::vector<T*> getAllRecursive() const;

	template<class T>
	std::shared_ptr<T> getShared() const;

	virtual sf::Vector2f getPosition() const; 
	virtual void setPosition(const sf::Vector2f& p); 
	void translate(const sf::Vector2f& p);

	bool isAligned(const char axis = 'b') const;

	/** Called after the constructor; all components should have been already
	 *  added at this time.
	 *  @return this
	 */
	virtual Game::Entity* init();
	/** Called every frame */
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

template<class T>
std::vector<T*> Entity::getAll() const {
	std::vector<T*> all;
	for (auto& comp : components) {
		Component *ptr = comp.get();
		T* derived = nullptr;
		if (ptr && (derived = dynamic_cast<T*>(ptr)))
			all.push_back(derived);
	}
	return all;
}

template<class T>
std::vector<T*> Entity::getAllRecursive() const {
	std::vector<T*> all;
	for (auto& comp : components) {
		Component *ptr = comp.get();
		T* derived = nullptr;
		if (ptr && (derived = dynamic_cast<T*>(ptr)))
			all.push_back(derived);
		// XXX: yuck! Is this cast safe? Is there a better way around this?
		auto sub = reinterpret_cast<Game::Entity*>(ptr)->getAllRecursive<T>();
		all.insert(all.end(), sub.begin(), sub.end());
	}
	return all;
}

template<class T>
std::shared_ptr<T> Entity::getShared() const {
	for (auto& comp : components) {
		Component *ptr = comp.get();
		T* derived = nullptr;
		if (ptr && (derived = dynamic_cast<T*>(ptr)))
			return std::static_pointer_cast<T>(comp);
	}
	return std::shared_ptr<T>();
}

}
