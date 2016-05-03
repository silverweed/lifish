#include "Animated.hpp"
#include "Game.hpp"
#include "utils.hpp"

using Game::Animated;

Animated::Animated(Game::Entity *const owner, const std::string& texture_name) 
	: Game::Component(owner)
{
	texture = Game::cache.loadTexture(texture_name);
	frameClock = addComponent(new Game::Clock<1>(this));
}

Animation& Animated::addAnimation(const std::string& name) {
	auto& anim = animations[name];
	anim.setSpriteSheet(*texture);

	return anim;
}

Animation& Animated::addAnimation(const std::string& name, 
		std::initializer_list<sf::IntRect> frames, bool set)
{
	auto& anim = addAnimation(name);
	
	for (auto& frame : frames)
		anim.addFrame(frame);

	if (set)
		setAnimation(anim);

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
	if (anim == nullptr)
		throw std::invalid_argument("Animation set to non-existing `" + name + "`!");

	animatedSprite.setAnimation(*anim);
}

void Animated::setAnimation(Animation& anim) {
	animatedSprite.setAnimation(anim);
}

void Animated::update() {
	animatedSprite.setPosition(owner->getPosition());
	animatedSprite.update(frameClock->restart());
}
