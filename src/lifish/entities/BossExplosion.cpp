#include "BossExplosion.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "Temporary.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "utils.hpp"
#include <random>

using lif::BossExplosion;
using lif::TILE_SIZE;

BossExplosion::BossExplosion(const sf::Vector2f& pos)
	: lif::OneShotFX(pos, "bossbullet.png", {
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
	})
{
	addComponent<lif::Sounded>(*this,
		lif::sid("explode"), lif::getAsset("sounds", "bullet101_hit.ogg")
	);
	get<lif::ZIndexed>()->setZIndex(lif::conf::zindex::BOSS_EXPLOSIONS);
	std::uniform_int_distribution<> dist(100, 232);
}
