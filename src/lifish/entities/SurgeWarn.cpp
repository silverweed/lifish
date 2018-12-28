#include "SurgeWarn.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "core.hpp"

using lif::SurgeWarn;

static const sf::Vector2f SIZE(1024, 64);

SurgeWarn::SurgeWarn(const sf::Vector2f& pos, const sf::Time& duration, const lif::Angle& rotation)
	: lif::Entity(pos)
	, duration(duration)
{
	sprite = addComponent<lif::Sprite>(*this, lif::getAsset("graphics", "surge_warn.png"),
			sf::IntRect(0, 0, SIZE.x, SIZE.y));
	sprite->getSprite().setColor(sf::Color(255, 255, 255, 20));
	sprite->getSprite().setOrigin(SIZE.x * 0.5, SIZE.y * 0.5);
	sprite->getSprite().setRotation(rotation.asDegrees());
	addComponent<lif::Temporary>(*this, [this] () {
		return t > this->duration;
	});
	addComponent<lif::Drawable>(*this, *sprite);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TALL_ENTITIES);
}

void SurgeWarn::update() {
	lif::Entity::update();

	t += lif::time.getDelta();
	sprite->getSprite().setColor(sf::Color(255, 255, 255, 20 + (t / duration) * 235));
}
