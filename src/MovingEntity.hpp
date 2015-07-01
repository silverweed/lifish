#pragma once
/**
 * Entities which can move (enemies, players, bullets, ...)
 */

#include <string>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"
#include "Animated.hpp"

namespace Game {

enum class Direction { UP, RIGHT, DOWN, LEFT };

class MovingEntity : public Game::Entity, public Game::Animated {
protected:
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	enum : unsigned short {
		ANIM_UP    = 0,
		ANIM_RIGHT = 1,
		ANIM_DOWN  = 2,
		ANIM_LEFT  = 3,
		ANIM_DEATH = 4,
		ANIM_WIN   = 5,
		ANIM_HURT  = 6
	};
	float speed;
public:
	MovingEntity(sf::Vector2f pos, const std::string& texture_name);

	virtual float getSpeed() const { return speed; }

	virtual void draw(sf::RenderTarget& window) override { Game::Animated::draw(window); }

	virtual void move(const Direction direction);
	virtual void stop();
};

}
