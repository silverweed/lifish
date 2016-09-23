#pragma once

#include "Entity.hpp"
#include "Collider.hpp"
#include "Killable.hpp"

namespace Game {

/** The base class for Bosses */
class Boss : public Game::Entity {
protected:
	/** A Collider can be implemented by children */
	Game::Collider *collider = nullptr;
	Game::Killable *killable = nullptr;

	bool isHurt = false, wasHurt = false;


	void _checkCollision(Game::Collider& coll);

	// Sets this boss as hurt for the next update, so that it can be e.g.
	// drawn in a different way for that frame.
	void _hurt();
public:
	explicit Boss(const sf::Vector2f& pos);

	void update() override;
};

}
