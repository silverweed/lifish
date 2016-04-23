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

template <class T>
T* EntityGroup::_commonAdd(T *entity, bool owned) {
	entities.push_back(std::unique_ptr<Game::Entity>(entity));
	if (!owned)
		unowned.insert(entity);	

	return entity;
}

template <class T>
T* EntityGroup::add(T *entity, bool owned) {
	return _commonAdd(entity, owned);
}
