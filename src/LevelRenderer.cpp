#include "LevelRenderer.hpp"
#include "LevelManager.hpp"

using Game::LevelRenderer;

LevelRenderer::LevelRenderer(const Game::LevelManager *owner)
	: owner(owner)
{}

void LevelRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	
	const auto level = owner->getLevel();
	if (level == nullptr) return;

	window.draw(level, states);

	const auto& entities = owner->getEntities();

	for (auto& entity : entities.all()) {
		auto drawable = entity->get<Game::Drawable>();
		if (drawable != nullptr)
			drawable->draw(window, states):
	}
/*
	// Draw fixed entities
	for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
		for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
			const unsigned short idx = top * LEVEL_WIDTH + left;
			const auto entity = fixedEntities[idx];
			if (entity == nullptr) continue;

			const auto e_type = level->getTile(left, top);
			switch (e_type) {
			case EntityType::BREAKABLE:
			case EntityType::TRANSPARENT_WALL:
				if (!entity->transparentTo.enemies) {
					// A breakable wall
					auto bw = static_cast<Game::BreakableWall*>(entity);
					if (bw->isDestroyed()) {
						// Chance to drop bonus
						const unsigned short bonus_type = Game::bonusTypeDistribution(rng);
						if (bonus_type >= Game::Bonus::N_BONUS_TYPES) {
							// no bonus
							fixedEntities[idx] = nullptr;
						} else {
							dropBonus(Game::tile(entity->getPosition()), bonus_type);
						}
						delete bw;
					} else {
						bw->draw(window);
					}
				} else {
					// A bonus
					auto bonus = static_cast<Game::Bonus*>(entity);
					if (bonus->isExpired()) {
						delete bonus;
						fixedEntities[idx] = nullptr;
					} else {
						bonus->draw(window);
					}
				}
				break;
			case EntityType::COIN:
				{
					auto coin = static_cast<Game::Coin*>(entity);
					if (coin->isGrabbed()) {
						delete coin;
						fixedEntities[idx] = nullptr;
					} else {
						coin->draw(window);
					}
					break;
				}
			default:
				entity->draw(window);
			}
		}
	}

	// Draw letters
	for (auto& letter : letters) {
		letter->draw(window);
	}

	// Draw bombs
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		for (unsigned short j = 0; j < bombs[i].size(); ++j) {
			auto bomb = bombs[i][j];
			if (bomb == nullptr) continue;
			if (bomb->isExploded()) {
				delete bomb;
				bombs[i][j] = nullptr;
			} else {
				bomb->draw(window);
			}
		}
	}
	
	// Draw moving entities
	for (auto it = movingEntities.begin(); it != movingEntities.end(); ) {
		auto entity = *it;
		if (entity->isDying() && !isPlayer(entity)) {
			entity->prepareDeathAnimation();
			if (entity->playDeathAnimation()) {
				if (extraGame)
					_spawnLetter(entity->getPosition());
				delete entity;
				it = movingEntities.erase(it);
				continue;
			}
		}
		entity->draw(window);
		++it;
	}

	// Draw bomb explosions
	for (auto it = explosions.begin(); it != explosions.end(); ) {
		if (!(*it)->isPlaying()) {
			delete *it;
			it = explosions.erase(it);
		} else {
			(*it)->draw(window);
			++it;
		}
	}

	// Draw bosses
	for (auto it = bosses.begin(); it != bosses.end(); ) {
		auto boss = *it;
		if (boss->isDead()) {
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
				if (players[i] == nullptr || players[i]->isDying())
					continue;
				Game::score[i] += boss->getPointsGiven();
			}

			_spawnPoints(boss->getPosition() + sf::Vector2f(TILE_SIZE, 0), boss->getPointsGiven(), true);
			delete boss;
			it = bosses.erase(it);
			continue;

		} else if (boss->isDying()) {
			if (boss->explClock.getElapsedTime().asMilliseconds() >= 100) {
				boss->explClock->restart();
				// Calculate a random location inside the boss
				const auto bpos = boss->getPosition();
				std::uniform_real_distribution<float> dist(-0.5 * TILE_SIZE,
						TILE_SIZE * (Game::Boss::SIZE - 0.5));
				const float x = dist(rng),
				            y = dist(rng);
				auto expl = new Game::BossExplosion(sf::Vector2f(bpos.x + x, bpos.y + y));
				_pushTemporary(expl);
				Game::cache.playSound(expl->getSoundFile());
			}
		}
		boss->draw(window);
		++it;
	}

	// Draw final boss
	if (_isFinalLevel() && finalBoss != nullptr) {
		finalBoss->draw(window);
		if (finalBoss->isDead()) {
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
				if (players[i] == nullptr || players[i]->isDying())
					continue;
				Game::score[i] += finalBoss->getPointsGiven();
			}

			_spawnPoints(finalBoss->getPosition() + sf::Vector2f(
					Game::FinalBoss::SIZE / 2 * TILE_SIZE, 0), finalBoss->getPointsGiven(), true);

			finalBoss.reset();

		} else if (finalBoss->isDying()) {
			if (finalBoss->explClock.getElapsedTime().asMilliseconds() >= 100) {
				finalBoss->explClock->restart();
				// Calculate a random location inside the finalBoss
				const auto bpos = finalBoss->getPosition();
				std::uniform_real_distribution<float> dist(-0.5 * TILE_SIZE, 
						TILE_SIZE * (Game::Boss::SIZE - 0.5));
				const float x = dist(rng),
				            y = dist(rng);
				auto expl = new Game::BossExplosion(sf::Vector2f(bpos.x + x, bpos.y + y));
				_pushTemporary(expl);
				Game::cache.playSound(expl->getSoundFile());
			}
			finalBoss->draw(window);
		} else {
			finalBoss->draw(window);
		}
	}
	
	// Draw other temporaries
	for (auto it = temporary.begin(); it != temporary.end(); ) {
		if (!(*it)->isPlaying()) {
			delete *it;
			it = temporary.erase(it);
		} else {
			(*it)->draw(window);
			++it;
		}
	}

	// Draw bullets
	for (auto it = bullets.begin(); it != bullets.end(); ) {
		auto bullet = *it;
		if (bullet->isDestroyed()) {
			delete bullet;
			it = bullets.erase(it);
		} else {
			bullet->draw(window);
			++it;
		}
	}

	// Draw dropping texts
	if (hurryUpText.isPlaying()) {
		hurryUpText.move();
		window.draw(hurryUpText);
	}

	if (gameOverText.isPlaying()) {
		gameOverText.move();
		window.draw(gameOverText);
	} else if (gameOverText.getPosition().y > 0) {
		// Game over just finished playing
		gameOverEnded = true;
	}

	if (extraGameText.isPlaying()) {
		extraGameText.move();
		window.draw(extraGameText);
	}
	*/
}
