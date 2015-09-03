#include "Explosion.hpp"
#include "LevelRenderer.hpp"
#include "BreakableWall.hpp"
#include "Scored.hpp"
#include "Player.hpp"
#include "utils.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <list>

using Game::Explosion;
using Game::TILE_SIZE;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius, const Game::Player *const source) :
	Game::Temporary(pos, Game::getAsset("test", "explosion.png")),
	Game::Sounded({ Game::getAsset("sounds", "explosion.ogg") }),
	radius(_radius),
	sourcePlayer(source)
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
	animatedSprite.setFrameTime(sf::seconds(0.05));
	explosionH.setFrameTime(sf::seconds(0.05));
	explosionV.setFrameTime(sf::seconds(0.05));
	animatedSprite.setLooped(false);
	explosionH.setLooped(false);
	explosionV.setLooped(false);

	propagation.fill(0);
}

void Explosion::propagate(Game::LevelRenderer *const lr) {
	const sf::Vector2i m_tile = Game::tile(pos);
	bool propagating[] = { true, true, true, true };

	const auto fixed = lr->getFixedEntities();
	const auto level = lr->getLevel();
	const auto bosses = lr->getBosses();

	// Check for boss at the explosion center
	for (auto& boss : bosses) {
		if (!boss->isDying() && boss->occupies(m_tile)) {
			boss->decLife(1);
			if (boss->getLife() <= 0) {
				boss->kill();
				Game::cache.playSound(boss->getSoundFile(Game::Sounds::DEATH));
			} else {
				boss->hurt();
				Game::cache.playSound(boss->getSoundFile(Game::Sounds::HURT));
			}
		}
	}

	for (unsigned short dir = 0; dir < 4; ++dir) {	
		for (unsigned short d = 1; d <= radius; ++d) {
			if (!propagating[dir]) continue;

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
			
			if (new_tile.x < 1 || new_tile.x > LEVEL_WIDTH 
					|| new_tile.y < 1 || new_tile.y > LEVEL_HEIGHT) {
				propagating[dir] = false;
				continue;
			}

			Entity *fxd = fixed[(new_tile.y - 1) * LEVEL_WIDTH + (new_tile.x - 1)];
			if (fxd == nullptr || fxd->transparentTo.explosions) {
				// Check if bomb
				Game::Bomb *bomb = lr->getBombAt(new_tile.x, new_tile.y);
				if (bomb != nullptr && !bomb->isIgnited()) {
					bomb->setExploding();
					continue;
				}

				++propagation[dir];

				// Check if boss
				for (auto& boss : bosses) {
					if (!boss->isDying() && boss->occupies(new_tile)) {
						boss->decLife(1);
						if (boss->getLife() <= 0) {
							boss->kill();
							Game::cache.playSound(boss->getSoundFile(Game::Sounds::DEATH));
						} else {
							boss->hurt();
							Game::cache.playSound(boss->getSoundFile(Game::Sounds::HURT));
						}
					}
				}

			} else {
				// It's a wall 
				propagating[dir] = false;
				if (d == 1 && level->getTile(new_tile.x - 1, new_tile.y - 1) == Game::EntityType::BREAKABLE) {
					auto bw = static_cast<Game::BreakableWall*>(fxd);
					bw->destroy();
					Game::cache.playSound(bw->getSoundFile());
					lr->givePointsTo(sourcePlayer, bw->getPosition(), bw->getPointsGiven());
				}
			}
		}
	}
}

void Explosion::checkHit(Game::LevelRenderer *const lr) {
	std::array<std::vector<Game::LifedMovingEntity*>, 5> moving;

	const auto allmoving = lr->getMovingEntities();

	for (unsigned short i = 0; i < moving.size(); ++i)
		moving[i].reserve(allmoving.size());

	const sf::Vector2i m_tile = Game::tile(pos);
	const sf::FloatRect origin_box(pos.x, pos.y, TILE_SIZE, TILE_SIZE),
		            row_box(TILE_SIZE, pos.y, LEVEL_WIDTH * TILE_SIZE, TILE_SIZE),
		            col_box(pos.x, TILE_SIZE, TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

	// Skim moving entities and keep only those which may be hit by this explosion
	for (auto& e : allmoving) {
		const auto epos = e->getPosition();
		const sf::FloatRect e_box(epos.x, epos.y, TILE_SIZE, TILE_SIZE);

		const bool intersects_x = e_box.intersects(row_box),
		           intersects_y = e_box.intersects(col_box);

		if (intersects_x && intersects_y) {
			// the same tile as the explosion's origin
			moving[4].push_back(e); 
		} else if (intersects_x) {
			if (epos.x < pos.x) 
				moving[ANIM_LEFT].push_back(e);
			else
				moving[ANIM_RIGHT].push_back(e);
		} else if (intersects_y) {
			if (epos.y < pos.y) 
				moving[ANIM_UP].push_back(e);
			else 
				moving[ANIM_DOWN].push_back(e);
		}
	}

	auto tryHit = [this, lr] (Game::LifedMovingEntity *const e, const unsigned short d, const sf::FloatRect& expl_box) {
		if (e->hasShield() || e->isDying()) return;

		// Check if entity's bounding box intersects this tile
		const sf::FloatRect e_box(e->getPosition().x, e->getPosition().y, TILE_SIZE, TILE_SIZE);
		if (e_box.intersects(expl_box)) {
			e->decLife(1);
			if (lr->isPlayer(e)) {
				e->setHurt(true);
				if (animatedSprite.getCurrentFrame() == animatedSprite.getAnimation()->getSize() - 1)
					e->giveShield(Game::DAMAGE_SHIELD_TIME);
				else
					e->giveShield(Game::DAMAGE_SHIELD_TIME/40.);
			} else {
				const auto se = dynamic_cast<Game::Scored*>(e);
				if (se != nullptr)
					lr->givePointsTo(sourcePlayer, e->getPosition(), se->getPointsGiven());
			}
			if (e->getLife() <= 0) {
				e->kill();
				Game::cache.playSound(e->getSoundFile(Game::Sounds::DEATH));
			}
		}
	};

	for (auto& e : moving[4]) {
		tryHit(e, 0, origin_box);
	}
	for (unsigned short dir = 0; dir < 4; ++dir) {
		for (unsigned short d = 1; d <= propagation[dir]; ++d) {
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

			const sf::FloatRect expl_box(new_tile.x * TILE_SIZE,
					new_tile.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
			for (auto& e : moving[dir])
				tryHit(e, d, expl_box);
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
	const unsigned short h_start_tile = m_tile.x - propagation[ANIM_LEFT],
	                     h_end_tile = m_tile.x + propagation[ANIM_RIGHT];
	explosionH.update(frameTime);
	for (unsigned short i = h_start_tile; i <= h_end_tile; ++i) {
		if (i == m_tile.x) continue;
		explosionH.setPosition(i * TILE_SIZE, pos.y);
		window.draw(explosionH);
	}
	// Draw vertical
	const unsigned short v_start_tile = m_tile.y - propagation[ANIM_UP],
	                     v_end_tile = m_tile.y + propagation[ANIM_DOWN];
	explosionV.update(frameTime);
	for (unsigned short i = v_start_tile; i <= v_end_tile; ++i) {
		if (i == m_tile.y) continue;
		explosionV.setPosition(pos.x, i * TILE_SIZE);
		window.draw(explosionV);
	}
}
