#include "Animated.hpp"

using Game::Animated;

Animated::Animated(Game::Entity *const owner, const std::string& texture_name) 
	: Game::Component(owner)
{
	texture = addComponent(new Game::Texture(this, texture_name));
	frameClock = addComponent(new Game::Clock<1>(this));
}

Animation& Animated::addAnimation(const std::string& name, bool set) {
	auto& anim = animations[name];
	anim.setSpriteSheet(*texture->getTexture());
	if (set)
		animatedSprite.setAnimation(anim);

	return anim;
}

void Animated::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(animatedSprite, states);
}

Animation* Animated::getAnimation(const std::string& name) {
	auto it = animations.find(name);
	if (it == animations.end())
		return nullptr;

	return &it->second;
}

void Animated::setAnimation(const std::string& name) {
	auto anim = getAnimation(name);
	if (anim != nullptr)
		animatedSprite.setAnimation(*anim);
}

void Animated::update() {
	animatedSprite.setPosition(owner.getPosition());
	animatedSprite.update(frameClock->restart());
}
