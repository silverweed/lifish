#pragma once

#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include "Component.hpp"
#include "sid.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace lif {

/**
 * An Animated is a drawable object whose sprite has a certain
 * number of associated animations.
 */
class Animated : public lif::Component, public sf::Drawable {
protected:
	sf::Texture *texture;
	std::unordered_map<lif::StringId, Animation> animations;
	std::unordered_map<lif::StringId, sf::Time> frameTimes;
	sf::Time defaultFrameTime = sf::Time::Zero;
	AnimatedSprite animatedSprite;
	bool manualPosition = false;

public:
	COMP_NOT_UNIQUE

	explicit Animated(lif::Entity& owner, const std::string& texture_name);

	/** Adds a new empty animation to this Animated and returns it */
	Animation& addAnimation(StringId name);
	Animation& addAnimation(const char *name) { return addAnimation(lif::sid(name)); }
	/** Adds a new animation tagged `name` with frames described by `frames`, and optionally sets it active. */
	Animation& addAnimation(StringId name, std::initializer_list<sf::IntRect> frames, bool set = false);
	Animation& addAnimation(const char *name, std::initializer_list<sf::IntRect> frames, bool set = false) {
		return addAnimation(lif::sid(name), frames, set);
	}

	/** If an animation tagged `name` exists, returns it. Else returns nullptr */
	const Animation* getAnimation(StringId name) const;
	const Animation* getAnimation(const char *name) const { return getAnimation(lif::sid(name)); }

	StringId getAnimationName() const;
	bool hasAnimation(StringId name) const;
	bool hasAnimation(const char *name) const { return hasAnimation(lif::sid(name)); }

	/** Sets the current animation to the one tagged `name`, if any, and starts playing.
	 *  Throws if `name` doesn't exist.
	 */
	void setAnimation(StringId name);
	void setAnimation(const char *name) { setAnimation(lif::sid(name)); }
	/** Sets the current animation to `anim` and starts playing */
	void setAnimation(Animation& anim);

	/** Sets the default frame time of all animations */
	void setDefaultFrameTime(sf::Time time);
	/** Sets the default frame time of animation `name` (overrides default frame time) */
	void setFrameTime(StringId name, sf::Time time);
	void setFrameTime(const char *name, sf::Time time) { setFrameTime(lif::sid(name), time); }

	/** Returns true if cur animation == `name` and animatedSprite is playing */
	bool isPlaying(StringId name) const;
	bool isPlaying(const char *name) const { return isPlaying(lif::sid(name)); }

	sf::Texture* getTexture() const { return texture; }
	void setTexture(sf::Texture *t) { texture = t; }

	const AnimatedSprite& getSprite() const { return animatedSprite; }
	AnimatedSprite& getSprite() { return animatedSprite; }

	void update() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setOrigin(const sf::Vector2f& o) override {
		lif::Component::setOrigin(o);
		animatedSprite.setOrigin(o);
	}

	void setPosition(const sf::Vector2f& pos) override {
		lif::Component::setPosition(pos);
		manualPosition = true;
	}
};

}
