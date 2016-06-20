#include "Boss.hpp"
#include "Game.hpp"
#include "utils.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Killable.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "Collider.hpp"

using Game::Boss;

Boss::Boss(const sf::Vector2f& pos) : Game::Entity(pos) {
	addComponent(new Game::Collider(this, Game::Layers::BOSSES, sf::Vector2i(
					SIZE * Game::TILE_SIZE, SIZE * Game::TILE_SIZE)));
	addComponent(new Game::Lifed(this, MAX_LIFE));
	addComponent(new Game::Scored(this, VALUE));
	addComponent(new Game::Sounded(this, {
		Game::getAsset("test", "boss_death.ogg"),
		Game::getAsset("test", "boss_hurt.ogg") 
	}));
	sprite = addComponent(new Game::Sprite(this, Game::getAsset("test", "boss.png"), sf::IntRect(
					0, 0, SIZE * Game::TILE_SIZE, SIZE * Game::TILE_SIZE)));
	addComponent(new Game::Drawable(this, sprite));
	addComponent(new Game::Killable(this, [this] () {
		// TODO		
	}));

	hurtClock = addComponent(new Game::Clock(this));
	explClock = addComponent(new Game::Clock(this));

	shootingPoints[0] = pos + sf::Vector2f(34, 36);
	shootingPoints[1] = pos + sf::Vector2f(63, 36);
}

void Boss::update() {
	Game::Entity::update();

	if (dead) {
		const float s = hurtClock->getElapsedTime().asSeconds();
		const float diff = s - std::floor(s);
		if (4*diff - std::floor(4*diff) < 0.5)
			return;
	} else if (isHurt) {
		if (hurtClock->getElapsedTime().asMilliseconds() > 200) {
			sprite->getSprite().setColor(sf::Color(sf::Color::White));
			isHurt = false;
		} else {
			sprite->getSprite().setColor(sf::Color(200, 0, 0, 255));
		}
	}
}

void Boss::hurt() { 
	isHurt = true;
	hurtClock.restart(); 
}

void Boss::kill() {
	dead = true;
	sprite.setColor(sf::Color(sf::Color::White));
	hurtClock.restart();
}
