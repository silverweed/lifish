/// Entity template methods implementation

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
		auto sub = ptr->getAllRecursive<T>();
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
