#pragma once

#include <memory>
#include <list>
#include <array>
#include <iterator>
#include <unordered_set>
#include <functional>
#include <SFML/System/NonCopyable.hpp>
#include "Entity.hpp"
#include "Temporary.hpp"
#include "Killable.hpp"

namespace Game {

namespace {
	template<class T>
	struct identity {
		typedef T type;
	};

	template<class T, typename... Args>
	using AppliedFunc = typename identity<std::function<void(T*, Args...)>>::type;
}

/**
 * A container for Entities, providing convenient methods for operating
 * on all or a specific type of them.
 */
class EntityGroup final : public Game::WithOrigin, private sf::NonCopyable {
	/** All the entities (owning references) */
	std::list<std::unique_ptr<Game::Entity>> entities;

	/** The unowned entities, which are managed externally */
	std::unordered_set<Game::Entity*> unowned;

	/** The static entities, which are always grid-aligned and cannot move */
	//std::array<Game::Entity*, LEVEL_WIDTH * LEVEL_HEIGHT> staticEntities;

	/** The list of the temporary entities, which have a brief lifetime
	 *  and ought to be removed when their Temporary component tells they're expired.
	 */
	std::list<Game::Temporary*> temporary;

	/** The list of Killable entities who are being destroyed (those whose isKilled() is
	 *  true but isKillInProgress() is true as well).
	 */
	std::list<Game::Killable*> dying;

	
	template<class T>
	T* _commonAdd(T *entity, bool owned);

	/** Removes any expired temporary from both `temporary` and `entities`, and destroys them.
	 *  If an entity is Killable and its `isKillInProgress()` is true, puts it in `dying`
	 *  instead of immediately destroing it.
	 */
	void _removeExpiredTemporaries();

	/** Removes any expired Killable in `dying`. */
	void _removeDying();

public:
	/**
	 * Constructs the EntityGroup as the owner of its entities.
	 * One can specify a given entity is owned externally by specifying
	 * it in the add() method.
	 */
	EntityGroup() {}
	~EntityGroup();

	/** Applies a void(Args...) function to all entities of type T */
	template<class T, typename... Args>
	void apply(AppliedFunc<T, Args...> func, Args... args);

	/** Applies a void(Args...) function to all entities */
	template<typename...Args>
	void apply(AppliedFunc<Game::Entity, Args...> func, Args... args);

	template<class T>
	T* add(T *entity, bool owned = true);

	void setOrigin(const sf::Vector2f& origin) override;

	template<class T>
	size_t size() const;
	size_t size() const { return entities.size(); }

	void updateAll();
};

///// Implementation /////

template<class T, typename... Args>
void EntityGroup::apply(AppliedFunc<T, Args...> func, Args... args) {
	for (auto& e : entities) {
		T* t = dynamic_cast<T*>(e.get());
		if (t != nullptr)
			func(t, args...);
	}
}

template<typename... Args>
void EntityGroup::apply(AppliedFunc<Game::Entity, Args...> func, Args... args) {
	for (auto& e : entities)
		func(e.get(), args...);
}

template<class T>
T* EntityGroup::_commonAdd(T *entity, bool owned) {
	entities.push_back(std::unique_ptr<Game::Entity>(entity));
	if (!owned)
		unowned.insert(entity);	

	entity->setOrigin(origin);

	auto tmp = entity->template get<Game::Temporary>();
	if (tmp != nullptr)
		temporary.push_back(tmp);

	return entity;
}

template<class T>
T* EntityGroup::add(T *entity, bool owned) {
	return _commonAdd(entity, owned);
}

template<class T>
size_t EntityGroup::size() const {
	size_t sz = 0;
	for (auto& e : entities) {
		if (dynamic_cast<T*>(e.get()) != nullptr)
			++sz;
	}
	return sz;
}

}
