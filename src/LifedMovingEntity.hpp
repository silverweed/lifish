#pragma once

#include <initializer_list>
#include "Lifed.hpp"
#include "MovingEntity.hpp"
#include "Sounded.hpp"

namespace Game {

class LifedMovingEntity : public Game::MovingEntity, public Game::Lifed, public Game::Sounded {
public:
	LifedMovingEntity(const sf::Vector2f& pos, const std::string& texture_name, 
			const unsigned short life, std::initializer_list<std::string> sound_list)
		: Game::MovingEntity(pos, texture_name), Game::Lifed(life), Game::Sounded(sound_list) {}
};

}
