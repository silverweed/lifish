#pragma once

#include <memory>
#include <list>
#include <array>
#include <iterator>
#include <unordered_set>
#include "Entity.hpp"

namespace Game {

/**
 * A container for Entities, providing convenient methods for operating
 * on all or a specific type of them.
 */
class EntityGroup final : public Game::WithOrigin {
	/** All the entities (owning references) */
	std::list<std::unique_ptr<Game::Entity>> entities;

	/** The unowned entities, which are managed externally */
	std::unordered_set<Game::Entity*> unowned;

	/** The static entities, which are always grid-aligned and cannot move */
	//std::array<Game::Entity*, LEVEL_WIDTH * LEVEL_HEIGHT> staticEntities;

	
	template<class T>
	T* _commonAdd(T *entity, bool owned);

public:
	/**
	 * Constructs the EntityGroup as the owner of its entities.
	 * One can specify a given entity is owned externally by specifying
	 * it in the add() method.
	 */
	EntityGroup() {}
	~EntityGroup();

	std::iterator<std::input_iterator_tag, const Game::Entity*> all();

	template<class T>
	T* add(T *entity, bool owned = true);

	// TODO
	unsigned int getCoinsNum() const { return 1; }

	void setOrigin(const sf::Vector2f& origin) override;
};

}
