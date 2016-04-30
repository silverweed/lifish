#pragma once

#include <array>
#include <SFML/Window.hpp>
#include "Player.hpp"
#include "Direction.hpp"
#include "Game.hpp"
#include "AxisMoving.hpp"
#include "Controls.hpp"
#include "Options.hpp"

namespace Game {

template<size_t N>
void get_directions(sf::RenderWindow& window, const std::array<Game::Player*, N>& players) {

	static_assert(N <= Game::MAX_PLAYERS, "Passed more than possible max players to get_directions!");

	std::array<Game::Direction, N> dir;
	dir.fill(Game::Direction::NONE);

	for (unsigned int i = 0; i < N; ++i) {
		if (players[i] == nullptr) continue;

		if (window.hasFocus()) {
			if (Game::options.useJoystick[i] >= 0) {
				const short joystick = Game::options.useJoystick[i];
				const auto horizontal = sf::Joystick::getAxisPosition(joystick, sf::Joystick::X),
					   vertical = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Y);
				if (vertical < -Game::JOYSTICK_INPUT_THRESHOLD) 
					dir[i] = Game::Direction::UP;
				else if (vertical > Game::JOYSTICK_INPUT_THRESHOLD)
					dir[i] = Game::Direction::DOWN;
				else if (horizontal < -Game::JOYSTICK_INPUT_THRESHOLD)
					dir[i] = Game::Direction::LEFT;
				else if (horizontal > Game::JOYSTICK_INPUT_THRESHOLD)
					dir[i] = Game::Direction::RIGHT;
			} else {
				if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::UP]))
					dir[i] = Game::Direction::UP;
				else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::LEFT]))
					dir[i] = Game::Direction::LEFT;
				else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::DOWN]))
					dir[i] = Game::Direction::DOWN;
				else if (sf::Keyboard::isKeyPressed(Game::playerControls[i][Game::Control::RIGHT]))
					dir[i] = Game::Direction::RIGHT;
			}
		}

		if (players[i]->isAligned())
			players[i]->template get<Game::AxisMoving>()->setDirection(dir[i]);
	}
}

}
