#include "Animated.hpp"
#include "GameCache.hpp"
#include "Time.hpp"
#include "core.hpp"
#include "utils.hpp"

using lif::Animated;

Animated::Animated(lif::Entity& owner, const std::string& textureName)
	: lif::Component(owner)
{
	_declComponent<Animated>();
	texture = lif::cache.loadTexture(textureName);
}

Animation& Animated::addAnimation(lif::StringId name) {
	auto& anim = animations[name];
	anim.setSpriteSheet(*texture);

	return anim;
}

Animation& Animated::addAnimation(lif::StringId name, std::initializer_list<sf::IntRect> frames, bool set) {
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

const Animation* Animated::getAnimation(lif::StringId name) const {
	auto it = animations.find(name);
	if (it == animations.end())
		return nullptr;

	return &it->second;
}

lif::StringId Animated::getAnimationName() const {
	for (const auto& anim : animations)
		if (&anim.second == animatedSprite.getAnimation())
			return anim.first;
	return lif::sid("");
}

bool Animated::hasAnimation(lif::StringId name) const {
	return animations.find(name) != animations.end();
}

void Animated::setAnimation(lif::StringId name) {
	auto anim = const_cast<Animation*>(getAnimation(name));
	if (anim == nullptr)
		throw std::invalid_argument("Animation set to non-existing `" + lif::sidToString(name) + "`!");

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
	animatedSprite.update(lif::time.getDelta());
}

bool Animated::isPlaying(lif::StringId name) const {
	return getAnimation(name) == animatedSprite.getAnimation() && animatedSprite.isPlaying();
}

void Animated::setDefaultFrameTime(sf::Time time) {
	defaultFrameTime = time;
}

void Animated::setFrameTime(lif::StringId name, sf::Time time) {
	frameTimes[name] = time;
}
