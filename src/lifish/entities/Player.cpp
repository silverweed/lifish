#include "Player.hpp"
#include "Lifed.hpp"
#include "Sounded.hpp"
#include "Shooting.hpp"
#include "MovingAnimator.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "RegularEntityDeath.hpp"
#include "Bonusable.hpp"
#include "Drawable.hpp"
#include "Bullet.hpp"
#include "Explosion.hpp"
#include "Killable.hpp"
#include "Controllable.hpp"
#include "GameCache.hpp"
#include "ZIndexed.hpp"
#include "BufferedSpawner.hpp"
#include "ArmorFX.hpp"
#include "utils.hpp"
#include "conf/zindex.hpp"
#include <sstream>

using lif::Player;
using lif::TILE_SIZE;

const sf::Time Player::DEATH_TIME = sf::milliseconds(5000);

Player::Player(const sf::Vector2f& pos, const lif::PlayerInfo& info)
	: lif::Entity(pos)
	, info(info)
	, drawProxy(*this)
{
	_init();
}

Player::Player(const sf::Vector2f& pos, const int id)
	: lif::Entity(pos)
	, info(id)
	, drawProxy(*this)
{
	_init();
}

void Player::_init() {
	// Setup components
	addComponent<lif::Lifed>(*this, lif::conf::player::MAX_LIFE, [this] (int damage, int) {
		// on hurt
		if (damage > 0)
			_hurt();
	});
	addComponent<lif::Collider>(*this, [this] (lif::Collider& cld) {
		// on collision
		if (!killable->isKilled())
			_checkCollision(cld);
	}, lif::c_layers::PLAYERS);
	moving = addComponent<lif::AxisMoving>(*this, lif::conf::player::DEFAULT_SPEED);
	moving->setFastTurn(true);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", std::string("player") +
				lif::to_string(info.id) + std::string(".png")));
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::PLAYERS);
	addComponent<lif::Drawable>(*this, drawProxy);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("test", std::string("player")
					+ lif::to_string(info.id) + std::string("_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("test", std::string("player")
					+ lif::to_string(info.id) + std::string("_hurt.ogg"))),
		std::make_pair("win", lif::getAsset("test", std::string("player")
				+ lif::to_string(info.id) + std::string("_win.ogg"))),
	});
	killable = addComponent<lif::Killable>(*this, [this] () {
		// on kill
		_kill();
	}, [this] () {
		return death->isKillInProgress();
	});
	bonusable = addComponent<lif::Bonusable>(*this);
	movingAnimator = addComponent<lif::MovingAnimator>(*this);
	addComponent<lif::Controllable>(*this,
				lif::controls::players[info.id-1],
				lif::controls::useJoystick[info.id-1],
				lif::controls::joystickBombKey[info.id-1]);
	hurtClock = addComponent<lif::Clock>(*this);
	death = addComponent<lif::RegularEntityDeath>(*this, lif::conf::player::DEATH_TIME);
	addComponent<lif::BufferedSpawner>(*this);

	// Setup animations
	auto& a_down = animated->addAnimation("walk_down");
	auto& a_up = animated->addAnimation("walk_up");
	auto& a_right = animated->addAnimation("walk_right");
	auto& a_left = animated->addAnimation("walk_left");

	for (int i = 0; i < WALK_N_FRAMES; ++i) {
		a_down.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		a_up.addFrame(sf::IntRect(i * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
		a_right.addFrame(sf::IntRect(i * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
		a_left.addFrame(sf::IntRect(i * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}

	auto& a_death = animated->addAnimation("death");
	auto& a_win = animated->addAnimation("win");
	auto& a_hurt = animated->addAnimation("hurt");

	for (int i = 0; i < DEATH_N_FRAMES; ++i)
		a_death.addFrame(sf::IntRect(i * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_win.addFrame(sf::IntRect(DEATH_N_FRAMES * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_hurt.addFrame(sf::IntRect((DEATH_N_FRAMES + 1) * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& a_idle = animated->addAnimation("idle");
	for (int i = 0; i < IDLE_N_FRAMES; ++i)
		a_idle.addFrame(sf::IntRect((DEATH_N_FRAMES + 2) * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_idle.addFrame(sf::IntRect((DEATH_N_FRAMES + 3) * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animated->setDefaultFrameTime(sf::seconds(0.06));
	animated->setFrameTime("idle", sf::seconds(0.15));
	animated->setAnimation(a_idle);
	animatedSprite.setLooped(true, false);
	animatedSprite.play();
}

void Player::update() {
	lif::Entity::update();

	// Check for speedy bonus
	if (bonusable->hasBonus(lif::BonusType::SPEEDY) && !moving->isDashing())
		moving->setDashing(2);
	else if (moving->isDashing())
		moving->setDashing(0);

	if (killable->isKilled() && killable->timeSinceDeath() > lif::conf::player::DEATH_STOP_ANIM_TIME
			&& animated->getSprite().isPlaying())
	{
		animated->getSprite().stop();
		animated->getSprite().setFrame(1);
	} else if (animated->getAnimationName() == "hurt" &&
			hurtClock->getElapsedTime() > lif::conf::player::HURT_ANIM_DURATION)
	{
		const auto dir = moving->getDirection();
		if (dir != lif::Direction::NONE)
			animated->setAnimation("walk_" + lif::directionToString(dir));
		else {
			animated->setAnimation("idle");
			moving->stop();
		}
	}

	if (winning && animated->getAnimationName() == "idle")
		animated->setAnimation("win");
}

void Player::setWinning(bool b) {
	winning = b;
}

void Player::_kill() {
	get<lif::Controllable>()->setActive(false);
	get<lif::Bonusable>()->reset();
	info.reset(false);
	death->kill();
}

void Player::_checkCollision(lif::Collider& cld) {
	/* A Player collides with:
	 * - Enemies' bodies
	 * - Bosses' bodies
	 * - Explosions
	 * - Bullets
	 */
	using L = lif::c_layers::Layer;

	if (bonusable->hasBonus(lif::BonusType::SHIELD))
		return;

	auto damage = 0;
	switch (cld.getLayer()) {
	case L::ENEMIES:
	case L::ENEMIES_IGNORE_BREAKABLES:
		if (cld.getOwner().get<lif::Killable>()->isKilled())
			return;
		{
			const auto shooting = cld.getOwner().get<lif::Shooting>();
			damage = shooting->getAttack().contactDamage;
			break;
		}
	case L::EXPLOSIONS:
	case L::ENEMY_EXPLOSIONS:
		damage = static_cast<const lif::Explosion&>(cld.getOwner()).getDamage();
		break;
	case L::ENEMY_BULLETS:
	case L::BOSS_BULLETS:
		{
			auto& bullet = static_cast<lif::Bullet&>(cld.getOwnerRW());
			if (bullet.hasDealtDamage()) return;
			damage = bullet.getInfo().damage;
			bullet.dealDamage();
			break;
		}
	case L::BOSSES:
		damage = lif::conf::player::MAX_LIFE;
		break;
	default:
		return;
	}

	// Apply armor
	if (info.powers.armor > 0) {
		damage = std::max(1, damage - info.powers.armor);
		get<lif::BufferedSpawner>()->addSpawned(new lif::ArmorFX(position - sf::Vector2f(TILE_SIZE / 4, 0)));
	}

	auto lifed = get<lif::Lifed>();
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hurt"));
	if (lifed->decLife(damage) <= 0) {
		killable->kill();
		return;
	}

	// Give shield after receiving damage
	if (cld.getLayer() == L::EXPLOSIONS) {
		const auto& expl = static_cast<const lif::Explosion&>(cld.getOwner());
		const auto& sprite = expl.get<lif::Animated>()->getSprite();
		// Give the "long" shield only if this is the explosion's last frame
		if (sprite.getCurrentFrame() == sprite.getAnimation()->getSize() - 1)
			bonusable->giveBonus(lif::BonusType::SHIELD, lif::conf::player::DAMAGE_SHIELD_TIME);
		else
			bonusable->giveBonus(lif::BonusType::SHIELD, lif::conf::player::DAMAGE_SHIELD_TIME / 40.f);
	}
}

void Player::resurrect() {
	get<lif::Lifed>()->setLife(lif::conf::player::MAX_LIFE);
	moving->realign();
	killable->resurrect();
	death->resurrect();
	get<lif::Controllable>()->setActive(true);
}

void Player::_hurt() {
	animated->setAnimation("hurt");
	moving->block(lif::conf::player::HURT_ANIM_DURATION);
	hurtClock->restart();
	bonusable->giveBonus(lif::BonusType::SHIELD, lif::conf::player::DAMAGE_SHIELD_TIME);
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hurt"));
}

//// PlayerDrawProxy ////
lif::PlayerDrawProxy::PlayerDrawProxy(const Player& player)
	: player(player)
{}

void lif::PlayerDrawProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*player.animated, states);
	if (player.bonusable->hasBonus(lif::BonusType::SHIELD)) {
		const float s = player.bonusable->getElapsedTime(lif::BonusType::SHIELD).asSeconds();
		const float diff = s - std::floor(s);
		if (player.bonusable->getRemainingTime(lif::BonusType::SHIELD) > sf::seconds(3)
				|| 4 * diff - std::floor(4 * diff) < 0.5)
		{
			AnimatedSprite shieldSprite(player.animated->getSprite());
			// TODO: scale & offset
			shieldSprite.setColor(sf::Color(0, 255, 0, 180));
			target.draw(shieldSprite, states);
		}
	}
}
