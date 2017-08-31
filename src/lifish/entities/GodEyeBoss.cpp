#include "GodEyeBoss.hpp"
#include "Lifed.hpp"
#include "Animated.hpp"
#include "Controllable.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "Collider.hpp"
#include "Level.hpp"
#include "HurtDrawProxy.hpp"
#include "Player.hpp"
#include "FreeSighted.hpp"
#include "Clock.hpp"
//#include "LightSource.hpp"
#include "Sounded.hpp"
#include "Drawable.hpp"
#include "Enemy.hpp"
#include "Bonusable.hpp"
#include "game.hpp"
#include "conf/boss.hpp"
#include "camera_utils.hpp"
#include <cmath>
#include <random>

#include <iostream>

using lif::GodEyeBoss;
using lif::TILE_SIZE;

const sf::Vector2f SIZE(3 * TILE_SIZE, 3 * TILE_SIZE);
const sf::Vector2f SHIELD_SIZE(4 * TILE_SIZE, 4 * TILE_SIZE);

GodEyeBoss::GodEyeBoss(const sf::Vector2f& pos, lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
{
	spriteBg = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "godeyeboss.png"));
	spriteBg->addAnimation("idle", {
		sf::IntRect(0 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(0 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
	}, true);
	spriteBg->getSprite().setFrameTime(sf::milliseconds(140));
	spriteFg = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "godeyeboss_fg.png"));
	spriteFg->addAnimation("idle", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	shield = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "godeyeshield.png"));
	shield->addAnimation("shield", {
		sf::IntRect(0 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(1 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(2 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(3 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(4 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(5 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
	}, true);
	shield->getSprite().setFrameTime(sf::milliseconds(110));
	shield->getSprite().setColor(sf::Color(255, 255, 255, 100));
	shield->setPosition(position + sf::Vector2f{ TILE_SIZE * 3/2, TILE_SIZE * 3/2 });
	shield->getSprite().setOrigin(SHIELD_SIZE * 0.5f);
	shieldAlphaClock = addComponent<lif::Clock>(*this);

	hurtDrawProxy = addComponent<lif::HurtDrawProxy>(*this, spriteBg);
	addComponent<lif::Lifed>(*this, lif::conf::boss::god_eye_boss::LIFE, [this] (int, int) {
		// on hurt
		if (++timesHurt == lif::conf::boss::god_eye_boss::TIMES_TO_HURT_BEFORE_DEATH)
			get<lif::Killable>()->kill();
	});
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("sounds", std::string("god_eye_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("sounds", std::string("god_eye_hurt.ogg")))
	});

	_addDefaultCollider(SIZE);
	// Boss is only vulnerable when shield is down
	vulnerable = false;

	attackClock = addComponent<lif::Clock>(*this);
	sighted = addComponent<lif::FreeSighted>(*this);
	//addComponent<lif::LightSource>(*this, 2 * TILE_SIZE, sf::Color(128, 128, 128))
			//->setPosition(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	addComponent<lif::Drawable>(*this, *this);
	addComponent<lif::Bonusable>(*this);
}

void GodEyeBoss::update() {
	lif::Boss::update();

	_updatePupilPos();
	_updateShield();

	//if (attackClock->getElapsedTime() > sf::seconds(10)) {
		//_shakeWalls();
		//attackClock->restart();
	//}

	//lm.getEntities().apply([] (const lif::Entity* e) {
			//if (!dynamic_cast<const lif::BreakableWall*>(e))return;
		//auto cld = e->get<lif::Collider>();
		//std::cout << "pos = " << e->getPosition() << ", cld = " << cld->getPosition() << std::endl;
	//});
}

void GodEyeBoss::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*hurtDrawProxy, states);
	target.draw(*spriteFg, states);
	target.draw(*shield, states);
}

void GodEyeBoss::_updatePupilPos() {
	sf::Vector2f newpos(position.x + SIZE.x / 3.f, position.y + SIZE.y / 3.f - 3);
	const auto player = sighted->nearest<lif::Player>();
	if (player != nullptr) {
		newpos += static_cast<float>(TILE_SIZE * 0.2) * lif::normalized(player->getPosition() - newpos);
	}
	spriteFg->setPosition(newpos);
}

void GodEyeBoss::_teleportTo(const sf::Vector2f& newpos) {
	position = newpos;
	_updatePupilPos();
	shield->setPosition(position + sf::Vector2f(TILE_SIZE * 3/2, TILE_SIZE * 3/2));
}

void GodEyeBoss::_updateShield() {
	// Boss becomes vulnerable as soon as there are no enemies in the level
	vulnerable = sighted->nearest<lif::Enemy>() == nullptr;
	if (!vulnerable) {
		shield->getSprite().setColor(sf::Color(255, 255, 255,
				100 + 30 * std::sin(5 * shieldAlphaClock->getElapsedTime().asSeconds())));
		const auto sc = 1 + 0.1 * std::sin(8 * shieldAlphaClock->getElapsedTime().asSeconds());
		shield->getSprite().setScale(sc, sc);
	} else {
		shield->getSprite().setColor(sf::Color(255, 255, 255, 0));
	}
}

void GodEyeBoss::_shakeWalls() {
	const auto SHAKE_DURATION = sf::seconds(3);

	lif::requestCameraShake(0.1, 50, 0.1, 50, SHAKE_DURATION, 2.0);

	// Freeze players
	for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player == nullptr)
			continue;
		player->setPosition(lif::aligned2(player->getPosition()));
		player->get<lif::Moving>()->stop();
		player->get<lif::Controllable>()->disableFor(SHAKE_DURATION);
	}

	// Change walls' disposition
	static std::uniform_int_distribution<> disposition(0, 2);
	const auto shakeType = disposition(lif::rng);
	lm.getEntities().apply([this, shakeType] (lif::Entity *e) {
		if (dynamic_cast<lif::FixedWall*>(e) == nullptr && dynamic_cast<lif::BreakableWall*>(e) == nullptr)
			return;

		const float width = lm.getLevel()->getInfo().width,
		            height = lm.getLevel()->getInfo().height;
		float x = e->getPosition().x,
		      y = e->getPosition().y;

		switch (shakeType) {
		case 0: // flip X
			x = (width + 1) * TILE_SIZE - x;
			break;
		case 1: // flix Y
			y = (height + 1) * TILE_SIZE - y;
			break;
		case 2: // flip both
			x = (width + 1) * TILE_SIZE - x;
			y = (height + 1) * TILE_SIZE - y;
			break;
		// TODO mirror diagonally (with a premade configuration)
		default:
			break;
		}
		e->setPosition(sf::Vector2f(x, y));
	});
}
