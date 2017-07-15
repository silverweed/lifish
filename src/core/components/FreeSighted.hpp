#pragma once

#include <algorithm>
#include <memory>
#include "Sighted.hpp"

namespace lif {

class FreeSighted : public lif::Sighted {
	using SeenEntitiesList = std::vector<std::pair<std::weak_ptr<lif::Entity>, float>>;

	SeenEntitiesList seen;

public:
	explicit FreeSighted(lif::Entity& owner, float visionRadius = -1);

	/** @return A list of pairs {entity, squared distance in pixel} */
	const SeenEntitiesList& entitiesSeen() const { return seen; }

	/** @return the nearest seen object of class `T` whose pointer is not expired. */
	template<class T>
	std::weak_ptr<T> nearest();
	
	void update() override;
};


template<class T>
std::weak_ptr<T> FreeSighted::nearest() {
	std::pair<std::weak_ptr<T>, float> cur(std::weak_ptr<T>(), 0);
	for (auto& e : seen) {
		if (e.first.expired()) continue;
		auto ptr = e.first.lock();
		if (std::dynamic_pointer_cast<const T>(ptr) != nullptr
				&& (cur.first.lock() == nullptr || cur.second > e.second))
		{
			cur = std::make_pair(std::static_pointer_cast<T>(ptr), e.second);
		}
	}
	return cur.first;
}

template<>
inline std::weak_ptr<lif::Entity> FreeSighted::nearest<lif::Entity>() {
	auto it = std::min_element(seen.begin(), seen.end(), [] (auto a, auto b) {
		return a.second < b.second;
	});
	if (it == seen.end()) return std::weak_ptr<lif::Entity>();
	return it->first;
}

}
