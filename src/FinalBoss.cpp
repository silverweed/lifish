#include "FinalBoss.hpp"
#include "Game.hpp"
#include "utils.hpp"

using Game::FinalBoss;
using Game::TILE_SIZE;

FinalBoss::FinalBoss(const sf::Vector2f& pos)
	: Game::LifedMovingEntity(pos, 
			Game::getAsset("test", "final_boss.png"), MAX_LIFE,
			{ Game::getAsset("test", "boss_death.ogg"), 
			Game::getAsset("test", "boss_hurt.ogg") })
	, Game::Scored(VALUE)
{
	transparentTo.bullets = true;

	for (unsigned short i = 0; i < 4; ++i) {
		animations[i].setSpriteSheet(texture);
		animations[i].addFrame(sf::IntRect(SIZE * TILE_SIZE, SIZE * TILE_SIZE * i,
				SIZE * TILE_SIZE, SIZE * TILE_SIZE));
		animations[i].addFrame(sf::IntRect(2 * SIZE * TILE_SIZE, SIZE * TILE_SIZE * i,
				SIZE * TILE_SIZE, SIZE * TILE_SIZE));
		animations[i].addFrame(sf::IntRect(3 * SIZE * TILE_SIZE, SIZE * TILE_SIZE * i,
				SIZE * TILE_SIZE, SIZE * TILE_SIZE));
		animations[i].addFrame(sf::IntRect(4 * SIZE * TILE_SIZE, SIZE * TILE_SIZE * i,
				SIZE * TILE_SIZE, SIZE * TILE_SIZE));
	}
	animatedSprite.setAnimation(animations[ANIM_LEFT]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.pause();

	_addClock(&explClock);
	_addClock(&hurtClock);
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
