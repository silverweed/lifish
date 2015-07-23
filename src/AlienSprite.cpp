#include "AlienSprite.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "utils.hpp"

using Game::AlienSprite;
using Game::Direction;

AlienSprite::AlienSprite() {
	Game::cache.loadTexture(texture, Game::getAsset("test", "aliensprite.png"));

	for (unsigned short i = 0; i < 4; ++i)
		animations[i].setSpriteSheet(texture);
	animations[ANIM_DEATH].setSpriteSheet(texture);

	animations[ANIM_DOWN].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DOWN].addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DOWN].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DOWN].addFrame(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_UP].addFrame(sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_UP].addFrame(sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_UP].addFrame(sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_UP].addFrame(sf::IntRect(7 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_RIGHT].addFrame(sf::IntRect(8 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_RIGHT].addFrame(sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_RIGHT].addFrame(sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_RIGHT].addFrame(sf::IntRect(2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_LEFT].addFrame(sf::IntRect(3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_LEFT].addFrame(sf::IntRect(4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_LEFT].addFrame(sf::IntRect(5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_LEFT].addFrame(sf::IntRect(6 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(7 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(8 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.play();
}

void AlienSprite::draw(sf::RenderTarget& window, const sf::Vector2f& pos, const Direction dir) {
	animatedSprite.setPosition(pos);
	unsigned short d = dir == Direction::NONE
				? ANIM_DEATH
				: Game::directionToUshort(dir);

	animatedSprite.play(animations[d]);
	animatedSprite.update(frameClock.restart());
	window.draw(animatedSprite);
}
