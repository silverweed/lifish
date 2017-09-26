#pragma once

#include <algorithm>
#include <memory>
#include "Sighted.hpp"

namespace lif {

class FreeSighted : public lif::Sighted {
	using SeenEntitiesList = std::vector<std::pair<lif::Entity*, float>>;

	SeenEntitiesList seen;

public:
	explicit FreeSighted(lif::Entity& owner, float visionRadius = -1);

	/** @return A list of pairs {entity, squared distance in pixel} */
	const SeenEntitiesList& entitiesSeen() const { return seen; }

	/** @return the nearest seen object of class `T` whose pointer is not expired. */
	template<class T>
	T* nearest();

	void update() override;
};


template<class T>
T* FreeSighted::nearest() {
	std::pair<T*, float> cur{nullptr, 0};
	for (auto& pair : seen) {
		auto e = pair.first;
		if (dynamic_cast<const T*>(e) != nullptr
				&& (cur.first == nullptr || cur.second > pair.second))
		{
			cur = std::make_pair(static_cast<T*>(e), pair.second);
		}
	}
	return cur.first;
}

template<>
inline lif::Entity* FreeSighted::nearest<lif::Entity>() {
	auto it = std::min_element(seen.begin(), seen.end(), [] (auto a, auto b) {
		return a.second < b.second;
	});
	if (it == seen.end()) return nullptr;
	return it->first;
}

}
