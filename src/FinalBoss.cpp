#include "FinalBoss.hpp"
#include "Game.hpp"
#include "utils.hpp"
#include "LevelRenderer.hpp"

using Game::FinalBoss;
using Game::TILE_SIZE;

FinalBoss::FinalBoss(const sf::Vector2f& pos)
	: Game::LifedMovingEntity(pos, 
			Game::getAsset("test", "final_boss.png"), MAX_LIFE,
			{ Game::getAsset("test", "boss_death.ogg") 
			, Game::getAsset("test", "boss_hurt.ogg") })
	, Game::Scored(VALUE)
{
	transparentTo.bullets = true;

	speed = 150.f;

	for (unsigned short i = 0; i < 4; ++i) {
		animations[i].setSpriteSheet(*texture);
		animations[ANIM_DOWN].addFrame(sf::IntRect(
				i * SIZE * TILE_SIZE,
				0, 
				SIZE * TILE_SIZE, 
				SIZE * TILE_SIZE));
		animations[ANIM_UP].addFrame(sf::IntRect(
				i * SIZE * TILE_SIZE, 
				SIZE * TILE_SIZE, 
				SIZE * TILE_SIZE, 
				SIZE * TILE_SIZE));
		animations[ANIM_RIGHT].addFrame(sf::IntRect(
				i * SIZE * TILE_SIZE, 
				2 * SIZE * TILE_SIZE,
				SIZE * TILE_SIZE, 
				SIZE * TILE_SIZE));
		animations[ANIM_LEFT].addFrame(sf::IntRect(
				i * SIZE * TILE_SIZE, 
				3 * SIZE * TILE_SIZE,
				SIZE * TILE_SIZE, 
				SIZE * TILE_SIZE));
	}
	animatedSprite.setAnimation(animations[ANIM_LEFT]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.pause();

	_addClock(&explClock);
	_addClock(&hurtClock);

	direction = Game::Direction::LEFT;
}

void FinalBoss::draw(sf::RenderTarget& window) {
	if (dead) {
		const float s = hurtClock.getElapsedTime().asSeconds();
		const float diff = s - std::floor(s);
		if (4*diff - std::floor(4*diff) < 0.5)
			return;
	} else if (isHurt) {
		if (hurtClock.getElapsedTime().asMilliseconds() > 200) {
			animatedSprite.setColor(sf::Color(sf::Color::White));
			isHurt = false;
		} else {
			animatedSprite.setColor(sf::Color(200, 0, 0, 255));
		}
	}
	Game::LifedMovingEntity::draw(window);
}

bool FinalBoss::occupies(const sf::Vector2i& tile) const {
	const auto m_tile = Game::tile(pos);
	return tile.x >= m_tile.x && tile.x < m_tile.x + SIZE 
		&& tile.y >= m_tile.y && tile.y < m_tile.y + SIZE;
}

void FinalBoss::hurt() { 
	isHurt = true;
	hurtClock.restart(); 
}

void FinalBoss::kill() {
	dead = true;
	animatedSprite.setColor(sf::Color(sf::Color::White));
	hurtClock.restart();
}

void FinalBoss::chooseDirection(const Game::LevelRenderer *const lr) {
	const auto cur_align = Game::tile(pos);
	if ((prevAlign == cur_align || distTravelled < MIN_DIST_BEFORE_CHANGE_DIR) && !colliding) 
		return;

	distTravelled = 0;
	const Game::Direction opp = Game::oppositeDirection(direction);

	// colliding with a moving entity
	if (colliding && canGo(direction, lr)) {
		direction = opp;
		return;
	}

	direction = Game::selectRandomViable(this, lr, opp);
}

bool FinalBoss::canGo(const Game::Direction dir, const Game::LevelRenderer *const lr) const {
	short iposx = (short)(pos.x / TILE_SIZE) - 1,
	      iposy = (short)(pos.y / TILE_SIZE) - 1;

	switch (dir) {
	case Direction::UP:
		--iposy;
		break;
	case Direction::LEFT:
		--iposx;
		break;
	case Direction::DOWN:
		++iposy;
		break;
	case Direction::RIGHT:
		++iposx;
		break;
	default: 
		return true;
	}

	if (iposx < 0 || iposx + SIZE >= LEVEL_WIDTH 
			|| iposy < 0 || iposy + SIZE >= LEVEL_HEIGHT)
		return false;

	const auto& fixed = lr->getFixedEntities();
	// Check all the appropriate side
	for (int i = 0; i < SIZE; ++i) {
		const short idx = 
			dir == Direction::UP ? iposy * LEVEL_WIDTH + iposx + i :
			dir == Direction::LEFT ? (iposy + i) * LEVEL_WIDTH + iposx :
			dir == Direction::DOWN ? (iposy + SIZE) * LEVEL_WIDTH + iposx + i :
			(iposy + i ) * LEVEL_WIDTH + iposx + SIZE;

		if (fixed[idx] != nullptr && !_isTransparentTo(fixed[idx]))
			return false;
	}

	// We don't treat the intersection with other bosses, as the game doesn't expect this case.
	return true;
}

void FinalBoss::detectCollisions(const Game::LevelRenderer *const lr) {
	colliding = false;

	if (isOverBoundaries(direction)) {
		colliding = true;
		return;
	}

	if (isAtLimit(direction)) {
		colliding = !canGo(direction, lr);
	}
}

void FinalBoss::breed() {
	breedClock.reset();
	
	// TODO
}
