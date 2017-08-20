#include "Sprite.hpp"
#include "GameCache.hpp"
#include "core.hpp"

using lif::Sprite;

Sprite::Sprite(lif::Entity& owner, const std::string& texture_name)
	: lif::Component(owner)
{
	_declComponent<Sprite>();
	texture = lif::cache.loadTexture(texture_name);
	sprite.setTexture(*texture);
}

Sprite::Sprite(lif::Entity& owner, const std::string& texture_name,
		const sf::IntRect& division)
	: Sprite(owner, texture_name)
{
	sprite.setTextureRect(division);
}

lif::Entity* Sprite::init() {
	lif::Component::init();
	sprite.setPosition(owner.getPosition());
	return this;
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(sprite, states);
}

void Sprite::update() {
	lif::Component::update();
	if (manualPosition)
		sprite.setPosition(position);
	else
		sprite.setPosition(owner.getPosition());
}

void Sprite::setOrigin(const sf::Vector2f& o) {
	lif::Component::setOrigin(o);
	sprite.setOrigin(o);
}
