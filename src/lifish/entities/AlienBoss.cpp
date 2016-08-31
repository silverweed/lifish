#include "AlienBoss.hpp"
#include "Player.hpp"
#include "Collider.hpp"
#include "Sounded.hpp"
#include "Drawable.hpp"
#include "Scored.hpp"
#include "Sprite.hpp"

using Game::AlienBoss;
 
const sf::Vector2i AlienBoss::SIZE = sf::Vector2i(3 * Game::TILE_SIZE, 3 * Game::TILE_SIZE);

AlienBoss::AlienBoss(const sf::Vector2f& pos)
	: Game::Boss(pos)
{
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("alienboss_death.ogg")),
		Game::getAsset("test", std::string("alienboss_hurt.ogg"))
	}));
	addComponent(new Game::Collider(*this, [this] (Game::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, Game::Layers::BOSSES, SIZE));
	addComponent(new Game::Killable(*this, [this] () {
		// on kill
		_kill();
	}, [this] () {
		// kill in progress
		// TODO
		return false;
	}));
	addComponent(new Game::Scored(*this, Game::Conf::Boss::ALIEN_BOSS_VALUE));
	addComponent(new Game::Drawable(*this, *addComponent(new Game::Sprite(*this, 
				Game::getAsset("test", "alien_boss.png"), sf::IntRect(0, 0, SIZE.x, SIZE.y)))));
}

void AlienBoss::_kill() {
	// TODO
}
