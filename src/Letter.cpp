#include "Letter.hpp"
#include "Game.hpp"
#include <random>

using Game::Letter;
using Game::TILE_SIZE;

static std::uniform_int_distribution<unsigned short> dist(0, Game::N_EXTRA_LETTERS - 1);

unsigned short Letter::randomId() {
	return dist(Game::rng);
}

Letter::Letter(const sf::Vector2f& pos, unsigned short _id) :
	Game::FixedEntity(pos, Game::getAsset("test", "extra_letters.png")),
	Game::Scored(100),
	id(_id)
{
	// Letters are indexed 0 to N_EXTRA_LETTERS - 1.
	if (id > Game::N_EXTRA_LETTERS - 1) 
		id = Game::N_EXTRA_LETTERS - 1;

	for (unsigned short i = 0; i < Game::N_EXTRA_LETTERS; ++i) {
		animations[i].setSpriteSheet(texture);
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
					((idx % (Game::N_EXTRA_LETTERS * 4)) / 10)  * TILE_SIZE,
					TILE_SIZE, TILE_SIZE));
		}
	}

	animatedSprite.setAnimation(animations[id]);
	animatedSprite.setPosition(pos);
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.1));
	animatedSprite.pause();
}

void Letter::checkTransition() {
	if (animatedSprite.isPlaying()) {
		animatedSprite.update(frameClock.restart());
		return;
	} else if (transitioning) {
		transitioning = false;
		id = (id + 1) % Game::N_EXTRA_LETTERS;
		animatedSprite.setAnimation(animations[id]);
		animatedSprite.pause();
	} else if (transitionClock.getElapsedTime().asMilliseconds() >= TRANSITION_DELAY) {
		transitionClock.restart();
		transitioning = true;
		animatedSprite.play();
	}
}
