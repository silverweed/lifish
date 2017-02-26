#include "HurtDrawProxy.hpp"
#include "conf/global.hpp"
#include "Clock.hpp"
#include "Animated.hpp"

using lif::HurtDrawProxy;

HurtDrawProxy::HurtDrawProxy(lif::Entity& owner)
	: lif::Component(owner)
{
	_declComponent<HurtDrawProxy>();
	hurtClock = addComponent(std::make_shared<lif::Clock>(*this));
	hurtClock->add(lif::conf::HURT_TIME);
}

lif::Entity* HurtDrawProxy::init() {
	lif::Component::init();
	animated = owner.get<lif::Animated>();
	if (animated == nullptr)
		throw std::invalid_argument("Owner of HurtDrawProxy has no Animated!");
	return this;
}

void HurtDrawProxy::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(*animated, states);
	if (isHurt()) {
		const auto& sprite = animated->getSprite();
		sf::Sprite hurtSprite(*animated->getTexture(),
				sprite.getAnimation()->getFrame(sprite.getCurrentFrame()));
		hurtSprite.setOrigin(sprite.getOrigin());
		hurtSprite.setPosition(sprite.getPosition());
		hurtSprite.setRotation(sprite.getRotation());
		hurtSprite.setScale(sprite.getScale());
		hurtSprite.setColor(sf::Color(255, 0, 0, 180));
		window.draw(hurtSprite, states);
	}
}

void HurtDrawProxy::hurt() {
	hurtClock->restart();
}

bool HurtDrawProxy::isHurt() const {
	return hurtClock->getElapsedTime() < lif::conf::HURT_TIME;
}
