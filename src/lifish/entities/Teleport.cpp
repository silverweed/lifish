#include "Teleport.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Fixed.hpp"
#include "GameCache.hpp"
#include "OneShotFX.hpp"
#include "Sounded.hpp"
#include "Spawning.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/teleport.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"
#include "utils.hpp"

using lif::Teleport;
using lif::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	//addComponent<lif::Fixed>(*this);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "teleport.png"));
	addComponent<lif::Drawable>(*this, *animated);
	collider = addComponent<lif::Collider>(*this, lif::c_layers::TELEPORTS);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TELEPORTS);
	addComponent<lif::Spawning>(*this, [this] (const lif::Spawning&) {
		return mustSpawnFlash;
	}, [this] () {
		mustSpawnFlash = false;
		return new lif::OneShotFX(position, "flash.png", {
			sf::IntRect(0, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(0, 0, lif::TILE_SIZE, lif::TILE_SIZE)
		});
	});
	addComponent<lif::Sounded>(*this,
		lif::sid("warp"), lif::getAsset("sounds", "teleport.ogg")
	);

	auto& anim = animated->addAnimation("teleport");
	for (unsigned i = 0; i < N_ANIM_FRAMES; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.07));
	animatedSprite.play();
}

void Teleport::triggerWarpFx() {
	mustSpawnFlash = true;
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("warp"));
}
