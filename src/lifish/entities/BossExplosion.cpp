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
	: lif::Entity(pos)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("test", "bossbullet.png"));
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("explode", lif::getAsset("test", "bossbullet_hit.ogg")) 
	});
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::BOSS_EXPLOSIONS);

	animated->addAnimation("explosion", {
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();

	addComponent<lif::Temporary>(*this, [&animatedSprite] () {
		return !animatedSprite.isPlaying();
	});
}
