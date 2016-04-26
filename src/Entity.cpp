#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"
#include "Component.hpp"
#include <type_traits>

using Game::Entity;

template<class Base, class T>
static inline bool instanceof(const T*) {
	return std::is_base_of<Base, T>::value;
}

template<class T>
T* Entity::addComponent(T *comp) {
	components.push_back(std::unique_ptr<Game::Component>(comp));
}

template<class T>
T* Entity::get() const {
	for (auto& comp : components) {
		Component *ptr = comp.get();
		if (instanceof<T>(ptr))
			return static_cast<T*>(ptr);
	}
	return nullptr;
}
