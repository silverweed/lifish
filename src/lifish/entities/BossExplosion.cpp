#include "BossExplosion.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include "ZIndexed.hpp"
#include "Sounded.hpp"
#include "Temporary.hpp"
#include "conf/zindex.hpp"
#include "utils.hpp"

using lif::BossExplosion;
using lif::TILE_SIZE;

BossExplosion::BossExplosion(const sf::Vector2f& pos)
	: lif::OneShotFX(pos, lif::getAsset("placeholder", "bossbullet.png"), {
		sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(4 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(5 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(6 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
	})
{
	addComponent<lif::Sounded>(*this,
		lif::sid("explode"), lif::getAsset("test", "bossbullet_hit.ogg")
	);
	get<lif::ZIndexed>()->setZIndex(lif::conf::zindex::BOSS_EXPLOSIONS);
}
