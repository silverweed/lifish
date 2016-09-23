#pragma once

#include <algorithm>
#include "Sighted.hpp"

namespace Game {

class FreeSighted : public Game::Sighted {
	using SeenEntitiesList = std::vector<std::pair<const Game::Entity*, float>>;

	SeenEntitiesList seen;

public:
	explicit FreeSighted(Game::Entity& owner, float visionRadius = -1);

	/** @return A list of pairs {entity, squared distance in pixel} */
	const SeenEntitiesList& entitiesSeen() const { return seen; }

	template<class T>
	const T* nearest() const;
	
	void update() override;
};


template<class T>
const T* FreeSighted::nearest() const {
	std::pair<const Game::Entity*, float> cur(nullptr, 0);
	for (const auto& e : seen) {
		if (dynamic_cast<const T*>(e.first) != nullptr && (cur.first == nullptr || cur.second > e.second))
			cur = e;
	}
	return static_cast<const T*>(cur.first);
}

template<>
inline const Game::Entity* FreeSighted::nearest<Game::Entity>() const {
	using Pair = std::pair<const Game::Entity*, float>;
	auto it = std::min_element(seen.begin(), seen.end(), [] (Pair a, Pair b) {
		return a.second < b.second;
	});
	if (it == seen.end()) return nullptr;
	return it->first;
}

}
