#pragma once

#include "Lifed.hpp"
#include "MovingEntity.hpp"

namespace Game {

class LifedMovingEntity : public Game::MovingEntity, public Game::Lifed {
public:
	LifedMovingEntity(const sf::Vector2f& pos, const std::string& texture_name, const unsigned short life)
		: MovingEntity(pos, texture_name), Lifed(life) {}
};

}
