#pragma once

#include "Component.hpp"

namespace Game {

class CollisionDetector;

class Collider : public Game::Component {
	friend class Game::CollisionDetector;

	Game::Collider *colliding = nullptr;
	const sf::Vector2i size;
	/** Collision layer */
	int layer;
	std::function<void(Game::Collider*)> onCollision;

public:
	explicit Collider(Game::Entity *const owner, const sf::Vector2i& size, int layer = 0);
	explicit Collider(Game::Entity *const owner, const sf::Vector2i& size, 
			int layer, std::function<void(Game::Collider*)> onCollision);

	bool isColliding() const { return colliding != nullptr; }

	int getLayer() const { return layer; }
	void setLayer(int l) { layer = l; }

	bool contains(const Game::Collider& other) const {
		return false;
		// TODO
		//const auto 
		//return sf::IntRect(owner..contains(other.size);
	}

	void update() override;
};

}
