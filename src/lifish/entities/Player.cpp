#include "Player.hpp"
#include "Lifed.hpp"
#include "Sounded.hpp"
#include "Bonusable.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
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
{
	_init();
}

Player::Player(const sf::Vector2f& pos, const unsigned short id) 
	: Game::Entity(pos)
	, info(id)
{
	_init();
}

void Player::_init() {
	addComponent(new Game::Lifed(*this, Game::Conf::Player::MAX_LIFE));
	addComponent(new Game::Collider(*this, Game::Layers::PLAYERS));
	moving = addComponent(new Game::AxisMoving(*this, Game::Conf::Player::DEFAULT_SPEED));
	animated = addComponent(new Game::Animated(*this, Game::getAsset("graphics", std::string("player") +
				Game::to_string(info.id) + std::string(".png"))));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::Player));
	addComponent(new Game::Drawable(*this, *animated));
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("player") + Game::to_string(info.id) + std::string("_death.ogg")),
		Game::getAsset("test", std::string("player") + Game::to_string(info.id) + std::string("_hurt.ogg")),
		Game::getAsset("test", std::string("player") + Game::to_string(info.id) + std::string("_win.ogg")),
	}));
	addComponent(new Game::Killable(*this, [this] () { _kill(); }));
	addComponent(new Game::Bonusable(*this));
	movingAnimator = addComponent(new Game::MovingAnimator(*this));
	addComponent(new Game::Controllable(*this, Game::Controls::players[info.id-1]));

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

void Player::_kill() {
	//MovingEntity::kill();
	get<Game::Lifed>()->setLife(0);
	get<Game::Bonusable>()->reset();
	info.reset();
}

// TODO
//void Player::move(const Game::Direction dir) {
	//MovingEntity::move(dir);
	//if (colliding)
		//realign();
//}

void Player::resurrect() {
	auto& animatedSprite = get<Game::Animated>()->getSprite();
	animatedSprite.setAnimation(*get<Game::Animated>()->getAnimation("down"));
	animatedSprite.pause();
	get<Game::Lifed>()->setLife(Game::Conf::Player::MAX_LIFE);
	moving->realign();
}

//bool Player::playDeathAnimation() {
	//float time = deathClock.getElapsedTime().asMilliseconds();
	//if (time > 3000) {
		//animatedSprite.setFrame(1);
		//animatedSprite.pause();
	//} else {
		//animatedSprite.update(frameClock.restart());
	//}
	//return time < DEATH_TIME;
//}

//void Player::stop() {
	//MovingEntity::stop();
	//if (winning) {
		//animatedSprite.setAnimation(animations[ANIM_WIN]);
		//animatedSprite.update(frameClock.restart());
	//}
//}

//void Player::update() {
	//Game::Entity::update();
//}
