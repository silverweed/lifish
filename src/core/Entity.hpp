#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <algorithm>
#include <utility>
#include <SFML/System.hpp>
#include "Activable.hpp"
#include "WithOrigin.hpp"
#include "Stringable.hpp"

namespace lif {

class Component;

/**
 * Base class for game entities (walls, enemies, players, ...)
 */
class Entity : public lif::WithOrigin, public lif::Stringable {
protected:
	using CompKey = std::type_index;
	using CompVec = std::vector<std::shared_ptr<lif::Component>>;

private:
	/** Used internally to fastly iterate over components only once. This is set up by init() */
	std::vector<lif::Component*> compSet;
	std::unordered_map<CompKey, CompVec> components;
	bool _initialized = false;

	std::string _toString(int indent) const;
	void _addUnique(lif::Component *c);

protected:
	sf::Vector2f position;

	template<class T>
	std::type_index _getKey() const {
		return std::type_index(typeid(T));
	}

public:
	explicit Entity();

	explicit Entity(const sf::Vector2f& pos);
	virtual ~Entity();

	template<class T>
	T* addComponent(const std::shared_ptr<T>& comp);

	/** Adds component of class T to this Entity and constructs it with `args`.
	 *  Throws std::logic_error if T is an unique component and an instance of T
	 *  is already among this entity's components.
	 *  @return A naked (unowning) pointer to the newly added component.
	 */
	template<class T, class... Args>
	T* addComponent(Args&&... args);

	/** @return An unowning pointer to the first component of type T added to this entity */
	template<class T>
	T* get() const;

	/** @return A shared pointer to the first component of type T added to this entity */
	template<class T>
	std::shared_ptr<T> getShared() const;

	/** @return A vector of unowning pointers to all components of type T whose owner is this entity */
	template<class T>
	std::vector<T*> getAll() const;

	/** @return A vector of shared pointers to all components of type T whose owner is this entity */
	template<class T>
	std::vector<std::shared_ptr<T>> getAllShared() const;

	/** @return Unowning pointers to all components of type T belonging to this entity and to all its components,
	 *  recursively. This method is expensive, so it should not be called frequently.
	 */
	template<class T>
	std::vector<T*> getAllRecursive() const;

	/** Called after the constructor; all components should have been already
	 *  added at this time.
	 *  Note: this method is automatically invoked by EntityGroup::add.
	 *  If the entity is never added to an EntityGroup, it must be invoked manually.
	 *  @return this
	 */
	virtual lif::Entity* init();
	/** Called every frame */
	virtual void update();

	/** Implements WithOrigin */
	virtual void setOrigin(const sf::Vector2f& origin) override;

	/** Implements Stringable */
	virtual std::string toString() const override;
	/** @return The human-readable name of this Entity's concrete class. */
	std::string getTypeName() const;

	// Position-related methods
	virtual sf::Vector2f getPosition() const;
	virtual void setPosition(const sf::Vector2f& p);
	void translate(const sf::Vector2f& p);
	bool isAligned(const char axis = 'b') const;
};

#define COMP_NOT_UNIQUE \
	static constexpr bool requiredUnique() { return false; }

/**
 * A generic component of a game entity. Inherit this to add behavior.
 */
class Component : public lif::Entity, public lif::Activable {
protected:
	lif::Entity& owner;
	/** A Component's keys vector keeps track of the Component's subclasses,
	 *  so it can be queried via its subclasses with Entity::get().
	 */
	std::vector<CompKey> keys;

	template<class T>
	void _declComponent() { keys.emplace_back(_getKey<T>()); }
public:
	/** If true, adding more than a component of this type to an Entity
	 *  will raise a logic_error.
	 */
	static constexpr bool requiredUnique() { return true; }

	explicit Component(lif::Entity& owner);

	/** Gets the owner of this component */
	const lif::Entity& getOwner() const { return owner; }
	/** Gets the owner of this component (non-const) */
	lif::Entity& getOwnerRW() const { return owner; }

	const std::vector<CompKey>& getKeys() const { return keys; }
};

#include "Entity.inl"

}
