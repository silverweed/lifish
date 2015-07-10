#include "Explosion.hpp"
#include "LevelRenderer.hpp"
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
	for (short i = 2; i > -1; --i) {
		animations[ANIM_DEATH].addFrame(sf::IntRect(i*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		animations[ANIM_UP].addFrame(sf::IntRect(i*TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
		animations[ANIM_LEFT].addFrame(sf::IntRect(i*TILE_SIZE, 2*TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[ANIM_DEATH]);
	explosionH.setAnimation(animations[ANIM_LEFT]);
	explosionV.setAnimation(animations[ANIM_UP]);
	animatedSprite.setFrameTime(sf::seconds(0.35));
	explosionH.setFrameTime(sf::seconds(0.35));
	explosionV.setFrameTime(sf::seconds(0.35));
	animatedSprite.setLooped(false);
	explosionH.setLooped(false);
	explosionV.setLooped(false);

	propagation.fill(0);
}

void Explosion::propagate(const LevelRenderer *const lr) {
	sf::Vector2i m_tile = Game::tile(pos);
	bool propagating[] = { true, true, true, true };

	auto fixed = lr->getFixedEntities();
	auto allmoving = lr->getMovingEntities();
	auto level = lr->getLevel();

	std::array<std::list<MovingEntity*>, 5> moving;
	// Skim moving entities and keep only those which may be hit by this explosion
	for (auto& e : allmoving) {
		sf::Vector2f epos = e->getPosition();
		if (epos.x != pos.x && epos.y != pos.y) {
			// this entity won't be affected by this explosion
			continue;
		}
		if (epos.x < pos.x) moving[ANIM_LEFT].push_back(e);
		else if (epos.x > pos.x) moving[ANIM_RIGHT].push_back(e);
		else if (epos.y < pos.y) moving[ANIM_UP].push_back(e);
		else if (epos.y > pos.y) moving[ANIM_DOWN].push_back(e);
		else moving[4].push_back(e); // the same tile as the explosion's origin
	}

	for (unsigned short d = 1; d <= radius; ++d) {
		for (unsigned short dir = 0; dir < 4; ++dir) {	
			if (propagating[dir]) {
				sf::Vector2i new_tile = m_tile;
				switch (dir) {
				case ANIM_UP:
					new_tile.y -= d;
					break;
				case ANIM_LEFT:
					new_tile.x -= d;
					break;
				case ANIM_DOWN:
					new_tile.y += d;
					break;
				case ANIM_RIGHT:
					new_tile.x += d;
					break;
				}
				Entity *fxd = fixed[new_tile.y * LEVEL_WIDTH + new_tile.x];
				if (fxd == nullptr || fxd->transparentTo.explosions) {
					sf::FloatRect expl_box(new_tile.x, new_tile.y, TILE_SIZE, TILE_SIZE);
					for (auto& e : moving[dir]) {
						// Check if entity's bounding box intersects this tile
						sf::FloatRect enemy_box(e->getPosition().x, e->getPosition().y, TILE_SIZE, TILE_SIZE);
						if (enemy_box.intersects(expl_box)) {
							// TODO: kill enemy
						}
					}
				} else {
					propagating[ANIM_UP] = false;
					if (level->getTile(new_tile.x, new_tile.y) == Game::EntityType::BREAKABLE) {
						// TODO: break
					} else {
						// TODO: check if bomb
					}
				}
			}
		}
	}
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
	explosionH.update(frameTime);
	for (unsigned short i = h_start_tile; i <= h_end_tile; ++i) {
		if (i == m_tile.x) continue;
		explosionH.setPosition(sf::Vector2f(i * TILE_SIZE, pos.y));
		window.draw(explosionH);
	}
	// Draw vertical
	unsigned short v_start_tile = std::max(1, m_tile.y - radius),
		       v_end_tile = std::min(static_cast<int>(LEVEL_HEIGHT), m_tile.y + radius);
	explosionV.update(frameTime);
	for (unsigned short i = v_start_tile; i <= v_end_tile; ++i) {
		if (i == m_tile.y) continue;
		explosionV.setPosition(sf::Vector2f(pos.x, i * TILE_SIZE));
		window.draw(explosionV);
	}
}
