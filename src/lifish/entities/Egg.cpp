#include "Egg.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "BufferedSpawner.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "EggCrack.hpp"
#include "EnemyFactory.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "LevelManager.hpp"
#include "Sounded.hpp"
#include "ZIndexed.hpp"
#include "collision_layers.hpp"
#include "conf/boss.hpp"
#include "conf/player.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::Egg;
using lif::TILE_SIZE;

static constexpr auto SPAWN_N_FRAMES = 4;

Egg::Egg(const sf::Vector2f& pos, lif::Direction dir, const lif::LevelManager& lm, unsigned short spawnedEnemyId)
	: lif::Entity(pos)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "egg.png"));
	auto& a_spawn_up = animated->addAnimation("spawn_up");
	auto& a_spawn_down = animated->addAnimation("spawn_down");
	auto& a_spawn_left = animated->addAnimation("spawn_left");
	auto& a_spawn_right = animated->addAnimation("spawn_right");
	for (unsigned i = 0; i < SPAWN_N_FRAMES; ++i) {
		a_spawn_up.addFrame(sf::IntRect(
				i * TILE_SIZE, 0 * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
		a_spawn_down.addFrame(sf::IntRect(
				(i + SPAWN_N_FRAMES) * TILE_SIZE, 0 * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
		a_spawn_left.addFrame(sf::IntRect(
				i * TILE_SIZE, 1 * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
		a_spawn_right.addFrame(sf::IntRect(
				(i + SPAWN_N_FRAMES) * TILE_SIZE, 1 * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	}
	a_spawn_up.addFrame(sf::IntRect((2 * SPAWN_N_FRAMES) * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_spawn_down.addFrame(sf::IntRect((2 * SPAWN_N_FRAMES) * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_spawn_left.addFrame(sf::IntRect((2 * SPAWN_N_FRAMES) * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_spawn_right.addFrame(sf::IntRect((2 * SPAWN_N_FRAMES) * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animated->getSprite().setLooped(false, false);
	switch (dir) {
		using D = lif::Direction;
	case D::UP:
		animated->setAnimation(a_spawn_up);
		break;
	case D::DOWN:
		animated->setAnimation(a_spawn_down);
		break;
	case D::LEFT:
		animated->setAnimation(a_spawn_left);
		break;
	case D::RIGHT:
		animated->setAnimation(a_spawn_right);
		break;
	default:
		throw std::invalid_argument("Invalid direction for Egg!");
	}
	animated->getSprite().setFrameTime(sf::seconds(0.18));
	animated->getSprite().play();

	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TALL_ENTITIES);

	addComponent<lif::Drawable>(*this, *animated);

	addComponent<lif::BufferedSpawner>(*this);

	addComponent<lif::Killable>(*this, [this, &lm, spawnedEnemyId] () {
		// on kill
		lif::cache.playSound(get<lif::Sounded>()->getSoundFile("crack"));
		auto spawner = get<lif::BufferedSpawner>();
		spawner->addSpawned(lif::EnemyFactory::create(lm, spawnedEnemyId, position));
		spawner->addSpawned(new lif::EggCrack(position - sf::Vector2f(0.5 * TILE_SIZE, 0.5 * TILE_SIZE)));
	});

	addComponent<lif::Sounded>(*this,
		lif::sid("spawn"), lif::getAsset("sounds", "egg_spawn.ogg"),
		lif::sid("crack"), lif::getAsset("sounds", "egg_crack.ogg")
	);

	addComponent<lif::AxisMoving>(*this, 0, dir);

	collider = addComponent<lif::Collider>(*this, [this] (lif::Collider&) {
		get<lif::Killable>()->kill();
	}, lif::c_layers::ENEMIES);
	collider->setActive(false);
}

void Egg::update() {
	lif::Entity::update();

	if (wasAnimationPlaying && !animated->getSprite().isPlaying()) {
		auto moving = get<lif::AxisMoving>();
		moving->setSpeed(
			lif::conf::boss::big_alien_boss::EGG_SPEED * lif::conf::player::DEFAULT_SPEED);
		wasAnimationPlaying = false;
		switch (moving->getDirection()) {
			using D = lif::Direction;
		case D::UP:
			position.y -= 10;
			break;
		case D::DOWN:
			position.y += 10;
			break;
		case D::LEFT:
			position.x -= 10;
			break;
		case D::RIGHT:
			position.x += 10;
			break;
		default:
			break;
		}
		collider->setActive(true);
	}

	if (collider->isAtLimit())
		get<lif::Killable>()->kill();
}
