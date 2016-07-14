#include "Letter.hpp"
#include "Game.hpp"
#include "Scored.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "utils.hpp"
#include "game_values.hpp"
#include <random>

using Game::Letter;
using Game::TILE_SIZE;
using Game::Conf::Player::N_EXTRA_LETTERS;

const sf::Time Letter::TRANSITION_DELAY = sf::milliseconds(3000);

static std::uniform_int_distribution<unsigned short> dist(0, N_EXTRA_LETTERS - 1);

unsigned short Letter::randomId() {
	return dist(Game::rng);
}

Letter::Letter(const sf::Vector2f& pos, unsigned short _id)
	: Game::Entity(pos)
	, id(_id)
{
	addComponent(new Game::Scored(this, 100));
	addComponent(new Game::Sounded(this,{ Game::getAsset("test", "letter_grab.ogg") }));
	transitionClock = addComponent(new Game::Clock(this));
	auto animated = addComponent(new Game::Animated(this, Game::getAsset("test", "extra_letters.png")));
	addComponent(new Game::Drawable(this, animated));
	addComponent(new Game::Component(this));

	// Letters are indexed 0 to N_EXTRA_LETTERS - 1.
	if (id > N_EXTRA_LETTERS - 1) 
		id = N_EXTRA_LETTERS - 1;

	std::array<Animation, N_EXTRA_LETTERS> animations;

	for (unsigned short i = 0; i < N_EXTRA_LETTERS; ++i) {	
		animations[i] = animated->addAnimation(Game::to_string(i));
		// Total different frames are 4 * N_EXTRA_LETTERS
		// (full letter + 3 transitions to next, cyclic).
		// Here, animations[i] is _5_ frames long, because it contains:
		//   (initial letter) + (3-frames transition) + (final letter)
		// where (i-th final letter) and ((i+1)-th initial letter) are the same
		// frame. This way we can tell when the letter has ended its transition:
		// that is when animations[i].isPlaying() == false.
		for (unsigned short j = 0; j < 5; ++j) {
			const unsigned short idx = i * 4 + j;
			animations[i].addFrame(sf::IntRect(
					(idx % 10) * TILE_SIZE,
					((idx % (N_EXTRA_LETTERS * 4)) / 10)  * TILE_SIZE,
					TILE_SIZE, TILE_SIZE));
		}
	}

	animatedSprite = &animated->getSprite();
	animatedSprite->setAnimation(animations[id]);
	animatedSprite->setLooped(false);
	animatedSprite->setFrameTime(sf::seconds(0.1));
	animatedSprite->pause();
}

void Letter::update() {
	if (!animatedSprite->isPlaying() && transitioning) {
		transitioning = false;
		id = (id + 1) % N_EXTRA_LETTERS;
		animatedSprite->setAnimation(*get<Game::Animated>()->getAnimation(Game::to_string(id)));
		animatedSprite->pause();
	} else if (transitionClock->getElapsedTime() >= TRANSITION_DELAY) {
		transitionClock->restart();
		transitioning = true;
		animatedSprite->play();
	}
}
