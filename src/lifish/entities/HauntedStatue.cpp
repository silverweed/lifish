#include "HauntedStatue.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "core.hpp"
#include "ZIndexed.hpp"
#include "Fixed.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "collision_functions.hpp"
#include "collision_layers.hpp"
#include "conf/zindex.hpp"
#include "conf/boss.hpp"

using Game::HauntedStatue;
using Game::TILE_SIZE;

HauntedStatue::HauntedStatue(const sf::Vector2f& pos) : Game::Entity(pos) {
	auto animated = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "haunted_statue.png")));
	animated->addAnimation("death", {
		sf::IntRect(0, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, 2 * TILE_SIZE)
	}, true);
	animated->setOrigin(sf::Vector2f(0, TILE_SIZE));
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.2));
	animatedSprite.pause();

	addComponent(new Game::Killable(*this, [animated] () {
		// on kill
		animated->getSprite().play();
	}, [animated] () {
		// is kill in progress
		return animated->getSprite().isPlaying();
	}));
	addComponent(new Game::Lifed(*this, Game::Conf::Boss::HauntingSpiritBoss::HAUNTED_STATUE_LIFE));
	addComponent(new Game::Drawable(*this, *animated));
	auto hurt_by_explosion = Game::hurtByExplosions(*this, Game::CFO_TAKE_SINGLE_HIT | Game::CFO_ONLY_ADJACENT);
	addComponent(new Game::Collider(*this, [this, hurt_by_explosion] (Game::Collider& cld) {
		// on collision
		if (!possessed) return;
		hurt_by_explosion(cld);
	}, Game::Layers::BREAKABLES, sf::Vector2f(TILE_SIZE, TILE_SIZE)));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::TALL_ENTITIES));
	addComponent(new Game::Fixed(*this));
}
