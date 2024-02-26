/// Entity template methods implementation

template<class T>
T* Entity::addComponent(const std::shared_ptr<T>& comp) {
	if (T::requiredUnique() && get<T>() != nullptr)
		throw std::logic_error("Two components of type " +
				std::string(typeid(T).name()) + " were added to this Entity!");
	_addUnique(comp.get());
	for (const auto& t : comp->getKeys())
		components[t].emplace_back(comp);
	return comp.get();
}

template<class T, class... Args>
T* Entity::addComponent(Args&&... args) {
	return addComponent(std::make_shared<T>(std::forward<Args>(args)...));
}

template<class T>
std::shared_ptr<T> Entity::getShared() const {
	auto comp = components.find(_getKey<T>());
	if (comp == components.end() || comp->second.size() == 0)
		return std::shared_ptr<T>();
	return std::static_pointer_cast<T>(comp->second[0]);
}

template<class T>
T* Entity::get() const {
	return getShared<T>().get();
}

template<class T>
std::vector<std::shared_ptr<T>> Entity::getAllShared() const {
	std::vector<std::shared_ptr<T>> comps;
	auto pair = components.find(_getKey<T>());
	if (pair == components.end())
		return comps;
	const auto& compVec = pair->second;
	comps.reserve(compVec.size());
	for (const auto& c : pair->second)
		comps.emplace_back(std::static_pointer_cast<T>(c));
	return comps;
}

template<class T>
std::vector<T*> Entity::getAll() const {
	std::vector<T*> comps;
	auto pair = components.find(_getKey<T>());
	if (pair == components.end())
		return comps;
	const auto& compVec = pair->second;
	comps.reserve(compVec.size());
	for (const auto& c : compVec)
		comps.emplace_back(static_cast<T*>(c.get()));
	return comps;
}

template<class T>
std::vector<T*> Entity::getAllRecursive() const {
	std::vector<T*> all;
	for (auto ptr : compSet) {
		if (!ptr)
				continue;
		if (T* derived = dynamic_cast<T*>(ptr))
			all.emplace_back(derived);

		auto sub = ptr->getAllRecursive<T>();
		all.insert(all.end(), sub.begin(), sub.end());
	}
	return all;
}
