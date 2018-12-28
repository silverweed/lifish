#include "HurtDrawProxy.hpp"
#include "Animated.hpp"
#include "Time.hpp"
#include "conf/global.hpp"

using lif::HurtDrawProxy;

HurtDrawProxy::HurtDrawProxy(lif::Entity& owner, lif::Animated *animated)
	: lif::Component(owner)
{
	_declComponent<HurtDrawProxy>();
	hurtT = lif::conf::HURT_TIME;
	if (animated)
		this->animated = animated;
}

lif::Entity* HurtDrawProxy::init() {
	lif::Component::init();
	if (animated == nullptr) {
		animated = owner.get<lif::Animated>();
		if (animated == nullptr)
			throw std::invalid_argument("Owner of HurtDrawProxy has no Animated!");
	}
	return this;
}

void HurtDrawProxy::update() {
	lif::Component::update();

	hurtT += lif::time.getDelta();
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
	hurtT = sf::Time::Zero;
}

bool HurtDrawProxy::isHurt() const {
	return hurtT < lif::conf::HURT_TIME;
}
