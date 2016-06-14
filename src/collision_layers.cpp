#include "collision_layers.hpp"

#define INTERACT(a, b) \
	collide[a][b] = true; \
	collide[b][a] = true;

using L = Game::Layers::Layer;

Game::Matrix<bool, L::N_LAYERS, L::N_LAYERS> Game::Layers::collide;

void Game::Layers::init() {
	for (auto& l : collide)
		l.fill(false);

	INTERACT(L::PLAYERS, L::PLAYERS)
	INTERACT(L::PLAYERS, L::ENEMIES)
	INTERACT(L::PLAYERS, L::ENEMY_BULLETS)
	INTERACT(L::PLAYERS, L::BOSSES)
	INTERACT(L::PLAYERS, L::BOSS_BULLETS)
	INTERACT(L::PLAYERS, L::WALLS)
	INTERACT(L::PLAYERS, L::DEFAULT) // Letters, teleports, coins (XXX: separate layer?)
	INTERACT(L::ENEMIES, L::ENEMIES)
	INTERACT(L::ENEMIES, L::ENEMY_BULLETS)
	INTERACT(L::ENEMIES, L::BOSS_BULLETS)
	INTERACT(L::ENEMIES, L::BOSSES)
	INTERACT(L::ENEMIES, L::WALLS)
	INTERACT(L::BOSSES, L::ENEMY_BULLETS)
	INTERACT(L::BOSSES, L::BOSS_BULLETS)
	INTERACT(L::ENEMY_BULLETS, L::WALLS)
	INTERACT(L::BOSS_BULLETS, L::WALLS)
}
