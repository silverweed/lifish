#include "Explosion.hpp"
#include "LevelManager.hpp"
#include "BreakableWall.hpp"
#include "Scored.hpp"
#include "Player.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "Temporary.hpp"
#include "ZIndexed.hpp"
#include "Sounded.hpp"
#include "CompoundCollider.hpp"
#include <list>

using Game::Explosion;
using Game::TILE_SIZE;
using Game::Direction;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius, const Game::Player *const source)
	: Game::Entity(pos)
	, radius(_radius)
	, sourcePlayer(source)
{
	//collider = addComponent(new Game::Collider(*this, Game::Layers::EXPLOSIONS, 
				//sf::Vector2i(TILE_SIZE, TILE_SIZE)));
	addComponent(new Game::Sounded(*this, { Game::getAsset("sounds", "explosion.ogg") }));
	explosionC = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "explosionC.png")));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::EXPLOSIONS));
	explosionC->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	explosionV = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "explosionV.png")));
	explosionV->getTexture()->setRepeated(true);
	explosionH = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "explosionH.png")));
	explosionH->getTexture()->setRepeated(true);

	addComponent(new Game::Drawable(*this, *this));

	explosionC->getSprite().setFrameTime(sf::seconds(0.05));
	explosionH->getSprite().setFrameTime(sf::seconds(0.05));
	explosionV->getSprite().setFrameTime(sf::seconds(0.05));
	explosionC->getSprite().setLooped(false);
	explosionH->getSprite().setLooped(false);
	explosionV->getSprite().setLooped(false);

	addComponent(new Game::Temporary(*this, [this] () {
		return !explosionC->getSprite().isPlaying();
	}));

	propagation.fill(0);
}

Game::Explosion* Explosion::propagate(Game::LevelManager& lm) {
	const sf::Vector2i m_tile = Game::tile(position);
	bool propagating[] = { true, true, true, true };
	auto& entities = lm.getEntities();
	
	// TODO
	//propagation.fill(radius);
	//_checkHit(lm);

	//const auto fixed = lr->getFixedEntities();
	//const auto level = lr->getLevel();
	//const auto bosses = lr->getBosses();

	// TODO Check for boss at the explosion center
	//for (auto& boss : bosses) {
		//if (!boss->isDying() && boss->occupies(m_tile)) {
			//boss->decLife(1);
			//if (boss->getLife() <= 0) {
				//boss->kill();
				//Game::cache.playSound(boss->getSoundFile(Game::Sounds::DEATH));
			//} else {
				//boss->hurt();
				//Game::cache.playSound(boss->getSoundFile(Game::Sounds::HURT));
			//}
		//}
	//}

	for (unsigned short dir = 0; dir < 4; ++dir) {	
		for (unsigned short r = 1; r <= radius; ++r) {
			if (!propagating[dir]) continue;

			sf::Vector2i new_tile = m_tile;
			switch (dir) {
			case Direction::UP:
				new_tile.y -= r;
				break;
			case Direction::LEFT:
				new_tile.x -= r;
				break;
			case Direction::DOWN:
				new_tile.y += r;
				break;
			case Direction::RIGHT:
				new_tile.x += r;
				break;
			}
			
			if (new_tile.x < 1 || new_tile.x > Game::LEVEL_WIDTH 
					|| new_tile.y < 1 || new_tile.y > Game::LEVEL_HEIGHT) {
				propagating[dir] = false;
				continue;
			}

			++propagation[dir];

			// Check if a solid fixed entity blocks propagation in this direction
			auto fxdlist = entities.getFixedAt(new_tile.x, new_tile.y);
			for (const auto& fxd : fxdlist) {
				const auto fxdcld = fxd.get().get<Game::Collider>();
				if (fxdcld != nullptr && Game::Layers::solid[fxdcld->getLayer()][Game::Layers::EXPLOSIONS]) { 
					propagating[dir] = false;
					break;
				}
			}
		}
	}

	_setPropagatedAnims();
	explCollider = addComponent(new Game::CompoundCollider(*this, Game::Layers::EXPLOSIONS,
			// size
			sf::Vector2i(TILE_SIZE * (propagation[Direction::LEFT] + propagation[Direction::RIGHT] + 1),
				     TILE_SIZE),
			// offset
			sf::Vector2f(-TILE_SIZE * propagation[Direction::LEFT], 0),
		{ Game::Collider(*this, Game::Layers::EXPLOSIONS,
			// size
			sf::Vector2i(TILE_SIZE,
				     TILE_SIZE * (propagation[Direction::UP] + propagation[Direction::DOWN] + 1)),
			// offset
			sf::Vector2f(0, -TILE_SIZE * propagation[Direction::UP]))
		}));

	return this;
}

void Explosion::checkHit(Game::LevelManager& lm) {
	lm.getEntities().apply([this] (Game::Entity *e) {
		auto cld = e->get<Game::Collider>();
		if (cld == nullptr) return;
		if (!explCollider->contains(*cld)) return;
		if (explCollider->collidesWith(*cld))
			cld->addColliding(*explCollider);
	});
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
