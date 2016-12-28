#pragma once

#include <memory>
#include <vector>
#include <SFML/System.hpp>
#include "Activable.hpp"
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

	template<class T>
	std::vector<std::shared_ptr<T>> getAllShared() const;

	template<class T>
	std::vector<std::shared_ptr<T>> getAllRecursiveShared() const;

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

#define COMP_NOT_UNIQUE \
	static constexpr bool requiredUnique() { return false; }

/**
 * A generic component of a game entity. Inherit this to add behavior.
 */
class Component : public Game::Entity, public Game::Activable {
protected:
	Game::Entity& owner;

public:
	/** If true, adding more than a component of this type to an Entity
	 *  will raise a logic_error.
	 */
	static constexpr bool requiredUnique() { return true; }

	explicit Component(Game::Entity& owner);

	/** Gets the owner of this component */
	const Game::Entity& getOwner() const { return owner; }

	/** Gets the owner of this component (non-const) */
	Game::Entity& getOwnerRW() const { return owner; }
};

#include "Entity.impl.hpp"

}
