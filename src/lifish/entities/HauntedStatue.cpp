#include "HauntedStatue.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "core.hpp"
#include "HurtDrawProxy.hpp"
#include "ZIndexed.hpp"
#include "Fixed.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "collision_functions.hpp"
#include "collision_layers.hpp"
#include "conf/zindex.hpp"
#include "conf/boss.hpp"

using lif::HauntedStatue;
using lif::TILE_SIZE;

HauntedStatue::HauntedStatue(const sf::Vector2f& pos) : lif::Entity(pos) {
	animated = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "haunted_statue.png")));
	animated->addAnimation("death", {
		sf::IntRect(0 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(1 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE)
	}, true);
	animated->setOrigin(sf::Vector2f(0, TILE_SIZE));
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.2));
	animatedSprite.pause();

	spirit = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "haunted_statue_spirit.png")));
	spirit->addAnimation("wave", {
		sf::IntRect(0 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(1 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, 2 * TILE_SIZE)
	}, true);
	spirit->setOrigin(sf::Vector2f(0, TILE_SIZE));
	auto& spiritSprite = spirit->getSprite();
	spiritSprite.setLooped(true);
	spiritSprite.setFrameTime(sf::seconds(0.1));
	spiritSprite.play();

	addComponent(std::make_shared<lif::Killable>(*this, [this] () {
		// on kill
		animated->getSprite().play();
	}, [this] () {
		// is kill in progress
		return animated->getSprite().isPlaying();
	}));
	addComponent(std::make_shared<lif::Lifed>(*this, lif::conf::boss::haunting_spirit_boss::HAUNTED_STATUE_LIFE,
	[this] (int damage, int)
	{
		// on hurt
		if (damage > 0)
			get<lif::HurtDrawProxy>()->hurt();
	}));
	addComponent(std::make_shared<lif::Drawable>(*this, *this));
	auto hurt_by_explosions = lif::hurtByExplosions(*this, lif::CFO_TAKE_SINGLE_HIT | lif::CFO_ONLY_ADJACENT);
	addComponent(std::make_shared<lif::Collider>(*this, [this, hurt_by_explosions] (lif::Collider& cld) {
		// on collision
		if (!possessed) return;
		hurt_by_explosions(cld);
	}, lif::c_layers::BREAKABLES, sf::Vector2f(TILE_SIZE, TILE_SIZE)));
	addComponent(std::make_shared<lif::ZIndexed>(*this, lif::conf::zindex::TALL_ENTITIES));
	addComponent(std::make_shared<lif::Fixed>(*this));
	hurtProxy = addComponent(std::make_shared<lif::HurtDrawProxy>(*this));
}

void HauntedStatue::setSpiritColor(sf::Color c) {
	spirit->getSprite().setColor(c);
}

void HauntedStatue::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(*hurtProxy, states);
	if (possessed)
		window.draw(*spirit, states);
}
