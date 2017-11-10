#include "Animated.hpp"
#include "GameCache.hpp"
#include "Clock.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <iostream>

using lif::Animated;

Animated::Animated(lif::Entity& owner, const std::string& texture_name)
	: lif::Component(owner)
{
	_declComponent<Animated>();
	texture = lif::cache.loadTexture(texture_name);
	frameClock = addComponent<lif::Clock>(*this);
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
		setAnimation(name);

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

std::string Animated::getAnimationName() const {
	for (const auto& anim : animations)
		if (&anim.second == animatedSprite.getAnimation())
			return anim.first;
	return "";
}

bool Animated::hasAnimation(const std::string& name) const {
	return animations.find(name) != animations.end();
}

void Animated::setAnimation(const std::string& name) {
	auto anim = getAnimation(name);
	if (anim == nullptr)
		throw std::invalid_argument("Animation set to non-existing `" + name + "`!");

	setAnimation(*anim);

	// Set the frame time, if specified.
	auto it = frameTimes.find(name);
	if (it != frameTimes.end())
		animatedSprite.setFrameTime(it->second);
	else if (defaultFrameTime != sf::Time::Zero)
		animatedSprite.setFrameTime(defaultFrameTime);
}

void Animated::setAnimation(Animation& anim) {
	animatedSprite.setAnimation(anim);
}

void Animated::update() {
	lif::Component::update();
	if (manualPosition)
		animatedSprite.setPosition(position);
	else
		animatedSprite.setPosition(owner.getPosition());
	animatedSprite.update(frameClock->restart());
}

bool Animated::isPlaying(const std::string& name) {
	return getAnimation(name) == animatedSprite.getAnimation() && animatedSprite.isPlaying();
}

void Animated::setDefaultFrameTime(sf::Time time) {
	defaultFrameTime = time;
}

void Animated::setFrameTime(const std::string& name, sf::Time time) {
	std::cout << "frame time[" << name << "] = " << time.asSeconds() << std::endl;
	frameTimes[name] = time;
}
