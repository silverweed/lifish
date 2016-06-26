#include "Explosion.hpp"
#include "LevelManager.hpp"
#include "BreakableWall.hpp"
#include "Scored.hpp"
#include "Player.hpp"
#include "utils.hpp"
#include "Game.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "Temporary.hpp"
#include "Sounded.hpp"
#include <list>

using Game::Explosion;
using Game::TILE_SIZE;
using Game::Direction;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius, const Game::Player *const source)
	: Game::Entity(pos)
	, radius(_radius)
	, sourcePlayer(source)
{
	addComponent(new Game::Sounded(this, { Game::getAsset("sounds", "explosion.ogg") }));
	explosionC = addComponent(new Game::Animated(this, Game::getAsset("graphics", "explosionC.png")));
	explosionC->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	explosionV = addComponent(new Game::Animated(this, Game::getAsset("graphics", "explosionV.png")));
	explosionV->getTexture()->setRepeated(true);
	explosionH = addComponent(new Game::Animated(this, Game::getAsset("graphics", "explosionH.png")));
	explosionH->getTexture()->setRepeated(true);

	addComponent(new Game::Drawable(this, this));

	explosionC->getSprite().setFrameTime(sf::seconds(0.05));
	explosionH->getSprite().setFrameTime(sf::seconds(0.05));
	explosionV->getSprite().setFrameTime(sf::seconds(0.05));
	explosionC->getSprite().setLooped(false);
	explosionH->getSprite().setLooped(false);
	explosionV->getSprite().setLooped(false);

	addComponent(new Game::Temporary(this, [this] () {
		return !explosionC->getSprite().isPlaying();
	}));

	propagation.fill(0);
}

Game::Explosion* Explosion::propagate(Game::LevelManager& lm) {
	const sf::Vector2i m_tile = Game::tile(position);
	bool propagating[] = { true, true, true, true };
	
	// TODO
	propagation.fill(radius);
	_checkHit(lm);

#if 0
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
			case Direction::UP:
				new_tile.y -= d;
				break;
			case Direction::LEFT:
				new_tile.x -= d;
				break;
			case Direction::DOWN:
				new_tile.y += d;
				break;
			case Direction::RIGHT:
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
					bomb->ignite();
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
				const auto tile = level->getTile(new_tile.x - 1, new_tile.y - 1);
				if (d == 1 && (tile == Game::EntityType::BREAKABLE 
						 || tile == Game::EntityType::TRANSPARENT_WALL)) {
					auto bw = static_cast<Game::BreakableWall*>(fxd);
					bw->destroy();
					Game::cache.playSound(bw->getSoundFile());
					lr->givePointsTo(sourcePlayer, bw->getPosition(), bw->getPointsGiven());
				}
			}
		}
	}
#endif
	return this;
}

void Explosion::_checkHit(Game::LevelManager& lm) {
#if 0
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
				moving[Direction::LEFT].push_back(e);
			else
				moving[Direction::RIGHT].push_back(e);
		} else if (intersects_y) {
			if (epos.y < pos.y) 
				moving[Direction::UP].push_back(e);
			else 
				moving[Direction::DOWN].push_back(e);
		}
	}

	auto tryHit = [this, lr] (Game::LifedMovingEntity *const e, const sf::FloatRect& expl_box) {
		if (e->hasShield() || e->isDying()) return;

		// Check if entity's bounding box intersects this tile
		const auto epos = e->getPosition();
		const sf::FloatRect e_box(epos.x, epos.y, TILE_SIZE, TILE_SIZE);
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
					lr->givePointsTo(sourcePlayer, epos, se->getPointsGiven());
			}
			if (e->getLife() <= 0) {
				e->kill();
				Game::cache.playSound(e->getSoundFile(Game::Sounds::DEATH));
			}
		}
	};

	for (auto& e : moving[4]) {
		tryHit(e, origin_box);
	}
	for (unsigned short dir = 0; dir < 4; ++dir) {
		for (unsigned short d = 1; d <= propagation[dir]; ++d) {
			sf::Vector2i new_tile = m_tile;
			switch (dir) {
			case Direction::UP:
				new_tile.y -= d;
				break;
			case Direction::LEFT:
				new_tile.x -= d;
				break;
			case Direction::DOWN:
				new_tile.y += d;
				break;
			case Direction::RIGHT:
				new_tile.x += d;
				break;
			}

			const sf::FloatRect expl_box(new_tile.x * TILE_SIZE,
					new_tile.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
			for (auto& e : moving[dir])
				tryHit(e, expl_box);
		}
	}
#endif

	_setPropagatedAnims();
}

void Explosion::_setPropagatedAnims() {
	const unsigned short hsize = TILE_SIZE * (propagation[Direction::RIGHT] + propagation[Direction::LEFT] + 1);
	explosionH->addAnimation("explode", {
		sf::IntRect(0, 0, hsize, TILE_SIZE),
		sf::IntRect(0, TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 2 * TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 3 * TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 2 * TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 0, hsize, TILE_SIZE)
	}, true);
	const unsigned short vsize = TILE_SIZE * (propagation[Direction::DOWN] + propagation[Direction::UP] + 1);
	explosionV->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, vsize),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(0, 0, TILE_SIZE, vsize)
	}, true);
	explosionH->setPosition(position - sf::Vector2f(TILE_SIZE * propagation[Direction::LEFT], 0));
	explosionV->setPosition(position - sf::Vector2f(0, TILE_SIZE * propagation[Direction::UP]));
}

void Explosion::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	// Draw horizontal
	window.draw(*explosionH, states);
	// Draw vertical
	window.draw(*explosionV, states);
	// Draw center
	window.draw(*explosionC, states);
}
