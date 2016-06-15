#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Game.hpp"
#include "collision_layers.hpp"

namespace Game {

class CollisionDetector;

class Collider : public Game::Component {
	friend class Game::CollisionDetector;

	Game::Collider *colliding = nullptr;
	const sf::Vector2i size;
	/** Collision layer */
	Game::Layers::Layer layer;
	std::function<void(Game::Collider*)> onCollision;

public:
	explicit Collider(Game::Entity *const owner, 
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE));

	explicit Collider(Game::Entity *const owner,
			  std::function<void(Game::Collider*)> onCollision,
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE));

	Game::Collider* getColliding() const { return colliding; }

	Game::Layers::Layer getLayer() const { return layer; }
	void setLayer(Game::Layers::Layer l) { layer = l; }

	bool contains(const Game::Collider& other) const {
		return getRect().intersects(other.getRect());
	}

	sf::IntRect getRect() const {
		const auto pos = owner->getPosition();
		return sf::IntRect(pos.x, pos.y, size.x, size.y);
	}

	void update() override;
};

}
