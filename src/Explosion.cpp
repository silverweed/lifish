#include "Explosion.hpp"
#include "utils.hpp"

using Game::Explosion;
using Game::TILE_SIZE;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius)
	: Temporary(pos, Game::getAsset("test", "explosion.png")), radius(_radius)
{
	animations[ANIM_DEATH].setSpriteSheet(texture);
	animations[ANIM_UP].setSpriteSheet(texture);
	animations[ANIM_LEFT].setSpriteSheet(texture);

	// An Explosion has 12 sprites: 4 central, 4 horizontal and 4 vertical
	// Here, ANIM_UP is used for vertical, ANIM_LEFT for horizontal and
	// ANIM_DEATH for central.
	for (unsigned short i = 0; i < 4; ++i) {
		animations[ANIM_DEATH].addFrame(sf::IntRect(i*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		animations[ANIM_UP].addFrame(sf::IntRect(i*TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
		animations[ANIM_LEFT].addFrame(sf::IntRect(i*TILE_SIZE, 2*TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[ANIM_DEATH]);
	explosionH.setAnimation(animations[ANIM_LEFT]);
	explosionV.setAnimation(animations[ANIM_UP]);
	animatedSprite.setFrameTime(sf::seconds(0.05));
	explosionH.setFrameTime(sf::seconds(0.05));
	explosionV.setFrameTime(sf::seconds(0.05));
	animatedSprite.setLooped(false);
	explosionH.setLooped(false);
	explosionV.setLooped(false);
}

void Explosion::draw(sf::RenderTarget& window) {
	sf::Time frameTime = frameClock.restart();
	// Draw center
	animatedSprite.update(frameTime);
	window.draw(animatedSprite);
	// Draw horizontal
	sf::Vector2i m_tile = Game::tile(pos);
	unsigned short h_start_tile = std::max(1, m_tile.x - radius),
		       h_end_tile = std::min(static_cast<int>(LEVEL_WIDTH), m_tile.x + radius);
	for (unsigned short i = h_start_tile; i <= h_end_tile; ++i) {
		explosionH.setPosition(sf::Vector2f(i * TILE_SIZE, pos.y));
		explosionH.update(frameTime);
		window.draw(explosionH);
	}
	// Draw vertical
	unsigned short v_start_tile = std::max(1, m_tile.y - radius),
		       v_end_tile = std::min(static_cast<int>(LEVEL_HEIGHT), m_tile.y + radius);
	for (unsigned short i = v_start_tile; i <= v_end_tile; ++i) {
		explosionH.setPosition(sf::Vector2f(pos.x, i * TILE_SIZE));
		explosionH.update(frameTime);
		window.draw(explosionV);
	}
}
