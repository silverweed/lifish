#include "Player.hpp"
#include "Lifed.hpp"
#include "Sounded.hpp"
#include "Bonusable.hpp"
#include "Drawable.hpp"
#include "Bullet.hpp"
#include "Explosion.hpp"
#include "Killable.hpp"
#include "Controllable.hpp"
#include "ZIndexed.hpp"
#include "utils.hpp"
#include <sstream>

using Game::Player;
using Game::TILE_SIZE;

const sf::Time Player::DEATH_TIME = sf::milliseconds(5000); 

Player::Player(const sf::Vector2f& pos, const Game::PlayerInfo& info)
	: Game::Entity(pos)
	, info(info)
	, drawProxy(*this)
{
	_init();
}

Player::Player(const sf::Vector2f& pos, const unsigned short id) 
	: Game::Entity(pos)
	, info(id)
	, drawProxy(*this)
{
	_init();
}

void Player::_init() {
	addComponent(new Game::Lifed(*this, Game::Conf::Player::MAX_LIFE));
	addComponent(new Game::Collider(*this, [this] (Game::Collider& cld) {
		if (!killable->isKilled())
			_checkCollision(cld);
	}, Game::Layers::PLAYERS));
	moving = addComponent(new Game::AxisMoving(*this, Game::Conf::Player::DEFAULT_SPEED));
	animated = addComponent(new Game::Animated(*this, Game::getAsset("graphics", std::string("player") +
				Game::to_string(info.id) + std::string(".png"))));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::PLAYERS));
	addComponent(new Game::Drawable(*this, drawProxy));
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("player") + Game::to_string(info.id) + std::string("_death.ogg")),
		Game::getAsset("test", std::string("player") + Game::to_string(info.id) + std::string("_hurt.ogg")),
		Game::getAsset("test", std::string("player") + Game::to_string(info.id) + std::string("_win.ogg")),
	}));
	killable = addComponent(new Game::Killable(*this, [this] () { 
		_kill(); 
	}, [this] () {
		return death->isKillInProgress();
	}));
	bonusable = addComponent(new Game::Bonusable(*this));
	movingAnimator = addComponent(new Game::MovingAnimator(*this));
	addComponent(new Game::Controllable(*this, Game::Controls::players[info.id-1]));
	hurtClock = addComponent(new Game::Clock(*this));
	death = addComponent(new Game::RegularEntityDeath(*this, Game::Conf::Player::DEATH_TIME));

	auto& a_down = animated->addAnimation("walk_down");
	auto& a_up = animated->addAnimation("walk_up");
	auto& a_right = animated->addAnimation("walk_right");
	auto& a_left = animated->addAnimation("walk_left");

	for (unsigned short i = 0; i < WALK_N_FRAMES; ++i) {
		a_down.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		a_up.addFrame(sf::IntRect(i * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
		a_right.addFrame(sf::IntRect(i * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
		a_left.addFrame(sf::IntRect(i * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}

	auto& a_death = animated->addAnimation("death");
	auto& a_win = animated->addAnimation("win");
	auto& a_hurt = animated->addAnimation("hurt");

	for (unsigned short i = 0; i < DEATH_N_FRAMES; ++i)
		a_death.addFrame(sf::IntRect(i * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_win.addFrame(sf::IntRect(3 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_hurt.addFrame(sf::IntRect(4 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_down);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.pause();
}

void Player::update() {
	Game::Entity::update();

	// Check for speedy bonus
	if (bonusable->hasBonus(Game::BonusType::SPEEDY) && !moving->isDashing())
		moving->setDashing(true, 2);
	else if (moving->isDashing())
		moving->setDashing(false);

	if (killable->isKilled() && killable->timeSinceDeath() > Game::Conf::Player::DEATH_STOP_ANIM_TIME
			&& animated->getSprite().isPlaying())
	{
		animated->getSprite().stop();
		animated->getSprite().setFrame(1);
	} else if (animated->getAnimationName() == "hurt" && 
			hurtClock->getElapsedTime() > Game::Conf::Player::HURT_ANIM_DURATION)
	{
		const auto dir = moving->getDirection();
		if (dir != Game::Direction::NONE)
			animated->setAnimation("walk_" + Game::directionToString(dir));
		else {
			animated->setAnimation("walk_down");
			moving->stop();
		}
	}
}

void Player::_kill() {
	get<Game::Bonusable>()->reset();
	info.reset();
	death->kill();
}

void Player::_checkCollision(Game::Collider& cld) {
	/* A Player collides with:
	 * - Enemies' bodies
	 * - Bosses' bodies
	 * - Explosions
	 * - Bullets
	 */
	using L = Game::Layers::Layer;
	
	if (bonusable->hasBonus(Game::BonusType::SHIELD))
		return;

	unsigned short damage = 0;
	switch (cld.getLayer()) {
	case L::ENEMIES:
		if (cld.getOwner().get<Game::Killable>()->isKilled())
			return;
		// fallthrough
	case L::EXPLOSIONS:
		damage = 1;
		break;
	case L::ENEMY_BULLETS:
	case L::BOSS_BULLETS:
		{
			auto& bullet = static_cast<Game::Bullet&>(cld.getOwnerRW());
			if (bullet.hasDealtDamage()) return;
			damage = bullet.getDamage();
			bullet.dealDamage();
			break;
		}
	case L::BOSSES:
		damage = Game::Conf::Player::MAX_LIFE;
		break;
	default:
		return;
	}

	auto lifed = get<Game::Lifed>();
	Game::cache.playSound(get<Game::Sounded>()->getSoundFile(Game::Sounds::HURT));
	_hurt();
	if (lifed->decLife(damage) <= 0) {
		killable->kill();
		return;
	}

	// Give shield after receiving damage
	if (cld.getLayer() == L::EXPLOSIONS) {
		const auto& expl = static_cast<const Game::Explosion&>(cld.getOwner());
		const auto& sprite = expl.get<Game::Animated>()->getSprite();
		// Give the "long" shield only if this is the explosion's last frame
		if (sprite.getCurrentFrame() == sprite.getAnimation()->getSize() - 1) 
			bonusable->giveBonus(Game::BonusType::SHIELD, Game::Conf::Player::DAMAGE_SHIELD_TIME);
		else
			bonusable->giveBonus(Game::BonusType::SHIELD, Game::Conf::Player::DAMAGE_SHIELD_TIME / 40.f);
	} else {
		bonusable->giveBonus(Game::BonusType::SHIELD, Game::Conf::Player::DAMAGE_SHIELD_TIME);
	}
}

void Player::resurrect() {
	auto& animatedSprite = get<Game::Animated>()->getSprite();
	animatedSprite.setAnimation(*get<Game::Animated>()->getAnimation("walk_down"));
	animatedSprite.pause();
	get<Game::Lifed>()->setLife(Game::Conf::Player::MAX_LIFE);
	moving->realign();
	killable->resurrect();
}

void Player::_hurt() {
	animated->setAnimation("hurt");
	moving->block(Game::Conf::Player::HURT_ANIM_DURATION);
	hurtClock->restart();
}

//// PlayerDrawProxy ////
Game::PlayerDrawProxy::PlayerDrawProxy(const Player& player)
	: player(player)
{}

void Game::PlayerDrawProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*player.animated, states);
	if (player.bonusable->hasBonus(Game::BonusType::SHIELD)) {
		const float s = player.bonusable->getElapsedTime(Game::BonusType::SHIELD).asSeconds();
		const float diff = s - std::floor(s);
		if (player.bonusable->getRemainingTime(Game::BonusType::SHIELD) > sf::seconds(3)
				|| 4 * diff - std::floor(4 * diff) < 0.5)
		{
			AnimatedSprite shieldSprite(player.animated->getSprite());
			// TODO: scale & offset
			shieldSprite.setColor(sf::Color(0, 255, 0, 180));
			target.draw(shieldSprite, states);
		}
	}
}
