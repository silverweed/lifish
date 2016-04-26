#include "EntityGroup.hpp"
#include "Component.hpp"

using Game::EntityGroup;

EntityGroup::~EntityGroup() {
	// Don't destroy unowned entities
	for (auto& e : entities) {
		if (unowned.find(e.get()) != unowned.end())
			e.release();
	}
}

class EntityConstIterator : public std::iterator<std::input_iterator_tag, const Game::Entity*> {
	std::list<std::unique_ptr<Game::Entity>>::iterator m_it;
public:
	EntityConstIterator(std::list<std::unique_ptr<Game::Entity>>& entities)
		: m_it(entities.begin()) {}

	EntityConstIterator& operator++() {
		++m_it;
		return *this;
	}

	bool operator==(const EntityConstIterator& rhs) { return m_it == rhs.m_it; }
	bool operator!=(const EntityConstIterator& rhs) { return m_it != rhs.m_it; }
	const Game::Entity* operator*() { return static_cast<const Game::Entity*>(m_it->get()); }
};

std::iterator<std::input_iterator_tag, const Game::Entity*> EntityGroup::all() {
	return EntityConstIterator(entities);
}

template<class T>
T* EntityGroup::_commonAdd(T *entity, bool owned) {
	entities.push_back(std::unique_ptr<Game::Entity>(entity));
	if (!owned)
		unowned.insert(entity);	

	return entity;
}

template<class T>
T* EntityGroup::add(T *entity, bool owned) {
	return _commonAdd(entity, owned);
}
