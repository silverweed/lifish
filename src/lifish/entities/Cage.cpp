#include "Cage.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "Lifed.hpp"
#include "Collider.hpp"
#include "Fixed.hpp"
#include "HurtDrawProxy.hpp"
#include "Killable.hpp"
#include "game.hpp"
#include "conf/boss.hpp"
#include "conf/zindex.hpp"
#include "collision_functions.hpp"

using lif::Cage;
using lif::TILE_SIZE;

constexpr auto SIZE = 3 * TILE_SIZE;

Cage::Cage(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "cage.png"));
	animated->addAnimation("oscillate", {
		sf::IntRect(0 * SIZE, 0, SIZE, SIZE),
		sf::IntRect(1 * SIZE, 0, SIZE, SIZE),
		sf::IntRect(2 * SIZE, 0, SIZE, SIZE),
		sf::IntRect(3 * SIZE, 0, SIZE, SIZE)
	}, true);
	animated->getSprite().setFrameTime(sf::seconds(0.4));
	addComponent<lif::Lifed>(*this, lif::conf::boss::temple_boss::CAGE_LIFE, [this] (int dmg, int) {
		if (dmg > 0)
			get<lif::HurtDrawProxy>()->hurt();
	});
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TALL_ENTITIES);
	addComponent<lif::Killable>(*this, [] () {
		// TODO
	});
	addComponent<lif::Collider>(*this,
		lif::hurtByExplosions(*this, lif::CFO_ONLY_ADJACENT),
		lif::c_layers::UNBREAKABLES,
		sf::Vector2f(SIZE, SIZE));
	addComponent<lif::Fixed>(*this);
	addComponent<lif::Drawable>(*this, *addComponent<lif::HurtDrawProxy>(*this));
}
