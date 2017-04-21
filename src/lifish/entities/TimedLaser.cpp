#include "TimedLaser.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "Lifed.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "Collider.hpp"

using lif::TimedLaser;
using lif::TILE_SIZE;

TimedLaser::TimedLaser(unsigned short rowCol,
			TimedLaser::Orientation ori,
			const sf::Time& warnDuration,
			int _damage,
			std::initializer_list<lif::c_layers::Layer> _damagedLayers)
	: lif::Entity({0, 0})
	, rowCol(rowCol)
	, warnDuration(warnDuration)
	, damage(_damage)
	, damagedLayers(_damagedLayers)
{
	const bool isVert = ori == TimedLaser::Orientation::VERTICAL;

	clock = addComponent<lif::Clock>(*this);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "timed_laser.png"));
	animated->addAnimation("warn_blink", {
		sf::IntRect(0 * TILE_SIZE, isVert * TILE_SIZE, TILE_SIZE, TILE_SIZE),
		sf::IntRect(1 * TILE_SIZE, isVert * TILE_SIZE, TILE_SIZE, TILE_SIZE)
	}, true);
	auto& sprite = animated->getSprite();
	sprite.setLooped(true);
	sprite.setFrameTime(sf::seconds(0.08));
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::FLASHES);

	// Set the position
	sf::Vector2f bounds(TILE_SIZE, TILE_SIZE);
	if (isVert) {
		position = sf::Vector2f(TILE_SIZE * (rowCol + 1), TILE_SIZE);
		sprite.setScale(1, lif::GAME_HEIGHT);
		bounds.y *= lif::GAME_HEIGHT;
	} else {
		position = sf::Vector2f(TILE_SIZE, TILE_SIZE * (rowCol + 1));
		sprite.setScale(lif::GAME_WIDTH, 1);
		bounds.x *= lif::GAME_HEIGHT;
	}

	addComponent<lif::Collider>(*this, [this] (const lif::Collider& cld) {
		// on collision
		if (isWarning
			|| (damagedLayers.size() > 0
				&& std::find(damagedLayers.begin(), damagedLayers.end(), cld.getLayer())
					== damagedLayers.end())
			|| std::find(dmgEntities.begin(), dmgEntities.end(), &cld.getOwner()) != dmgEntities.end())
		{
			return;
		}
		auto& owner = cld.getOwnerRW();
		auto lifed = owner.get<lif::Lifed>();
		if (lifed == nullptr)
			return;
		lifed->decLife(damage);
		dmgEntities.emplace_back(&owner);
	}, lif::c_layers::DEFAULT, bounds);

	addComponent<lif::Temporary>(*this, [this] () {
		return animated->getSprite().getColor().a < 1;
	});
}

void TimedLaser::update() {
	lif::Entity::update();

	if (isWarning && clock->getElapsedTime() > warnDuration) {
		isWarning = false;
		animated->getSprite().stop();
	}

	if (!isWarning) {
		auto& sprite = animated->getSprite();
		auto col = sprite.getColor();
		col.a -= clock->restart().asSeconds();
		sprite.setColor(col);
	}
}
