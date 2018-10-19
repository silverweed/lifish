#include "Player.hpp"
#include "Animated.hpp"
#include "ArmorFX.hpp"
#include "AxisMoving.hpp"
#include "Bonusable.hpp"
#include "BufferedSpawner.hpp"
#include "Bullet.hpp"
#include "Clock.hpp"
#include "Collider.hpp"
#include "Controllable.hpp"
#include "Drawable.hpp"
#include "Explosion.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "MovingAnimator.hpp"
#include "RegularEntityDeath.hpp"
#include "Shooting.hpp"
#include "Sounded.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "utils.hpp"
#include <sstream>

using lif::Player;
using lif::TILE_SIZE;
using namespace std::literals::string_literals;

/** How long does the player stay dead before resurrecting */
static const sf::Time DEATH_TIME = sf::seconds(5);
static const sf::Time DEATH_FRAME_TIME = sf::milliseconds(100);
static const sf::Time IDLE_FRAME_TIME = sf::milliseconds(200);
static const sf::Time WIN_FRAME_TIME = sf::milliseconds(50);
static constexpr int WALK_N_FRAMES = 8;
static constexpr int DEATH_N_FRAMES = 3;
static constexpr int IDLE_N_FRAMES = 1;
static constexpr int WIN_N_FRAMES = 1;


Player::Player(const sf::Vector2f& pos, const lif::PlayerInfo& info)
	: lif::Entity(pos)
	, info(info)
	, drawProxy(*this)
{
	_init();
}

Player::Player(const sf::Vector2f& pos, int id)
	: lif::Entity(pos)
	, info(id)
	, drawProxy(*this)
{
	_init();
}

void Player::_init() {
	// Setup components
	addComponent<lif::Lifed>(*this, lif::conf::player::MAX_LIFE, [this] (int damage, int newLife) {
		// on hurt
		if (newLife <= 0)
			killable->kill();
		else if (damage > 0)
			_hurt();
	});
	addComponent<lif::Collider>(*this, [this] (lif::Collider& cld) {
		// on collision
		if (!killable->isKilled())
			_checkCollision(cld);
	}, lif::c_layers::PLAYERS);
	moving = addComponent<lif::AxisMoving>(*this, lif::conf::player::DEFAULT_SPEED);
	moving->setFastTurn(false);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "player"s +
				lif::to_string(info.id) + ".png"s));
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::PLAYERS);
	addComponent<lif::Drawable>(*this, drawProxy);
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "player"s + lif::to_string(info.id) + "_death.ogg"s),
		lif::sid("hurt"), lif::getAsset("sounds", "player"s + lif::to_string(info.id) + "_hurt.ogg"s),
		lif::sid("win"), lif::getAsset("sounds", "player"s + lif::to_string(info.id) + "_win.ogg"s)
	);
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

	_setupAnimations();
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
		// Reset animation after death
		animated->getSprite().stop();
		animated->getSprite().setFrame(1);
	} else if (isHurt && hurtClock->getElapsedTime() > lif::conf::player::HURT_ANIM_DURATION)
	{
		// Restore walk/idle animation after hurt
		isHurt = false;
		const auto dir = moving->getDirection();
		if (dir != lif::Direction::NONE)
			animated->setAnimation(lif::sid("walk_" + lif::directionToString(dir)));
		else {
			animated->setAnimation(lif::sid("idle_" +
				lif::directionToStringOr(moving->getPrevDirection(), "down")));
			moving->stop();
		}
	}

	if (!killable->isKilled() && winning && _isIdleAnim())
		animated->setAnimation("win");
}

bool Player::_isIdleAnim() const {
	static lif::StringId iup = lif::sid("idle_up"),
	                     ileft = lif::sid("idle_left"),
	                     idown = lif::sid("idle_down"),
	                     iright = lif::sid("idle_right");
	const auto anim = animated->getAnimationName();
	return anim == iup || anim == ileft || anim == idown || anim == iright;
}

void Player::setWinning(bool b) {
	winning = b;
	if (b) {
		animated->setAnimation("win");
	} else {
		animated->setAnimation("idle_down");
		moving->block(sf::Time::Zero);
	}
}

void Player::_kill() {
	isHurt = false;
	get<lif::Controllable>()->setActive(false);
	get<lif::Bonusable>()->reset();
	info.reset(false);
	animated->getSprite().setLooped(true);
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
	const auto layer = cld.getLayer();
	switch (layer) {
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
		damage = 1; // player explosions always deal 1 damage per tick
		break;
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
	default:
		return;
	}

	bool shortShield = false;
	// In case of Explosions, give the "long" shield only if this is the explosion's last frame
	if (layer == L::EXPLOSIONS) {
		const auto& expl = static_cast<const lif::Explosion&>(cld.getOwner());
		const auto& sprite = expl.get<lif::Animated>()->getSprite();
		shortShield = sprite.getCurrentFrame() != sprite.getAnimation()->getSize() - 3;
	}

	dealDamage(damage, layer == L::EXPLOSIONS, shortShield);
}

void Player::dealDamage(int damage, bool ignoreArmor, bool shortShield) {
	if (killable->isKilled()) return;

	// Apply armor
	if (!ignoreArmor && info.powers.armor > 0) {
		damage = std::max(1, damage - info.powers.armor);
		get<lif::BufferedSpawner>()->addSpawned(new lif::ArmorFX(position - sf::Vector2f(TILE_SIZE / 4, 0)));
	}

	auto lifed = get<lif::Lifed>();
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hurt"));
	lifed->decLife(damage);

	// Give shield after receiving damage
	bonusable->giveBonus(lif::BonusType::SHIELD,
			lif::conf::player::DAMAGE_SHIELD_TIME * (shortShield ? 0.025f : 1));
}

void Player::resurrect() {
	get<lif::Lifed>()->setLife(lif::conf::player::MAX_LIFE);
	moving->realign();
	killable->resurrect();
	animated->getSprite().setLooped(true, false);
	death->resurrect();
	get<lif::Controllable>()->setActive(true);
}

void Player::_hurt() {
	isHurt = true;
	animated->setAnimation(lif::sid("hurt_" + _getDirectionString()));
	moving->block(lif::conf::player::HURT_ANIM_DURATION);
	hurtClock->restart();
	bonusable->giveBonus(lif::BonusType::SHIELD, lif::conf::player::DAMAGE_SHIELD_TIME);
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hurt"));
}

std::string Player::_getDirectionString() const {
	const auto dir = moving->getDirection();
	if (dir != lif::Direction::NONE)
		return lif::directionToString(dir);

	return lif::directionToStringOr(moving->getPrevDirection(), "down");
}

void Player::_setupAnimations() {
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

	auto& a_death_down = animated->addAnimation("death_down");
	auto& a_death_up = animated->addAnimation("death_up");
	auto& a_death_right = animated->addAnimation("death_right");
	auto& a_death_left = animated->addAnimation("death_left");

	for (int i = 0; i < DEATH_N_FRAMES; ++i) {
		a_death_down.addFrame(sf::IntRect(
			i * TILE_SIZE,
			4 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
		a_death_up.addFrame(sf::IntRect(
			i * TILE_SIZE,
			4 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
		a_death_right.addFrame(sf::IntRect(
			i * TILE_SIZE,
			4 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
		a_death_left.addFrame(sf::IntRect(
			i * TILE_SIZE,
			4 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
	}

	auto& a_win = animated->addAnimation("win");
	for (int i = 0; i < WIN_N_FRAMES; ++i) {
		a_win.addFrame(sf::IntRect(
			3 * TILE_SIZE,
			4 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
	}

	auto& a_idle_up = animated->addAnimation("idle_up");
	auto& a_idle_down = animated->addAnimation("idle_down");
	auto& a_idle_left = animated->addAnimation("idle_left");
	auto& a_idle_right = animated->addAnimation("idle_right");
	for (int i = 0; i < IDLE_N_FRAMES; ++i) {
		a_idle_down.addFrame(sf::IntRect(
			i * TILE_SIZE,
			0 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
		a_idle_up.addFrame(sf::IntRect(
			(i + IDLE_N_FRAMES) * TILE_SIZE,
			0 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
		a_idle_right.addFrame(sf::IntRect(
			i * TILE_SIZE,
			0 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
		a_idle_left.addFrame(sf::IntRect(
			(i + IDLE_N_FRAMES) * TILE_SIZE,
			0 * TILE_SIZE,
			TILE_SIZE, TILE_SIZE));
	}

	auto& a_hurt_up = animated->addAnimation("hurt_up");
	auto& a_hurt_down = animated->addAnimation("hurt_down");
	auto& a_hurt_left = animated->addAnimation("hurt_left");
	auto& a_hurt_right = animated->addAnimation("hurt_right");
	a_hurt_down.addFrame(sf::IntRect(
		4 * TILE_SIZE,
		4 * TILE_SIZE,
		TILE_SIZE, TILE_SIZE));
	a_hurt_up.addFrame(sf::IntRect(
		4 * TILE_SIZE,
		4 * TILE_SIZE,
		TILE_SIZE, TILE_SIZE));
	a_hurt_right.addFrame(sf::IntRect(
		4 * TILE_SIZE,
		4 * TILE_SIZE,
		TILE_SIZE, TILE_SIZE));
	a_hurt_left.addFrame(sf::IntRect(
		4 * TILE_SIZE,
		4 * TILE_SIZE,
		TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animated->setDefaultFrameTime(sf::seconds(0.08));
	animated->setFrameTime("idle_down", IDLE_FRAME_TIME);
	animated->setFrameTime("idle_up", IDLE_FRAME_TIME);
	animated->setFrameTime("idle_left", IDLE_FRAME_TIME);
	animated->setFrameTime("idle_right", IDLE_FRAME_TIME);
	animated->setFrameTime("death_down", DEATH_FRAME_TIME);
	animated->setFrameTime("death_up", DEATH_FRAME_TIME);
	animated->setFrameTime("death_left", DEATH_FRAME_TIME);
	animated->setFrameTime("death_right", DEATH_FRAME_TIME);
	animated->setFrameTime("win", WIN_FRAME_TIME);
	animated->setAnimation("idle_down");
	animatedSprite.setLooped(true, false);
	animatedSprite.play();
}

void Player::setExtra(int n, bool e) {
	info.extra[n] = e;
	if (std::accumulate(std::begin(info.extra), std::end(info.extra), true, [] (bool acc, bool b) {
		return acc && b;
	}) == true)
	{
		setRemainingLives(info.remainingLives + 1);
		info.extra.fill(false);
		lif::cache.playSound(lif::getAsset("sounds", lif::EXTRA_LIFE_SOUND));
	}
}

//// PlayerDrawProxy ////
lif::PlayerDrawProxy::PlayerDrawProxy(const Player& player)
	: player(player)
{
	const std::string SHIELD_SHADER =
		"uniform sampler2D texture;                             "
		"                                                       "
		"void main() {                                          "
		"    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);"
		"                                                       "
		"    pixel.r = 0.0;                                    "
		"    pixel.g = 0.8;                                    "
		"    pixel.b = 0.0;                                    "
		"    pixel.a *= 0.5;                                    "
		"                                                       "
		"    gl_FragColor = pixel;                              "
		"}                                                      ";

	if (!shieldShader.loadFromMemory(SHIELD_SHADER, sf::Shader::Fragment)) {
		std::cerr << "[ ERROR ] Failed to load shield shader from memory!\n";
	}
}

void lif::PlayerDrawProxy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*player.animated, states);
	if (player.killable->isKilled() || !player.bonusable->hasBonus(lif::BonusType::SHIELD))
		return;

	const float s = player.bonusable->getElapsedTime(lif::BonusType::SHIELD).asSeconds();
	const float diff = s - std::floor(s);
	if (player.bonusable->getRemainingTime(lif::BonusType::SHIELD) > sf::seconds(3)
			|| 4 * diff - std::floor(4 * diff) < 0.5)
	{
		AnimatedSprite shieldSprite(player.animated->getSprite());
		// TODO: scale & offset
		shieldSprite.setScale(1.2, 1.2);
		shieldSprite.setPosition(shieldSprite.getPosition() - sf::Vector2f(3, 3));
		if (shieldShader.isAvailable())
			target.draw(shieldSprite, &shieldShader);
		else {
			shieldSprite.setColor(sf::Color(0, 235, 0, 180));
			target.draw(shieldSprite, states);
		}
	}
}
