#include "game.hpp"
#include "LoopingMusic.hpp"
#include "collision_layers.hpp"
#include <cstring>
#include <iostream>
#include <array>
#include "game_values.hpp"

std::array<unsigned int, Game::MAX_PLAYERS> Game::score;
std::array<unsigned short, Game::MAX_PLAYERS> Game::playerContinues;
LoopingMusic *Game::music = nullptr;

bool Game::initGame() {
	if (!Game::init()) return false;
	Game::playerContinues.fill(Game::Conf::Player::INITIAL_CONTINUES);

	// Setup collision layers
	Game::Layers::init();

	return true;
}
