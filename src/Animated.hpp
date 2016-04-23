#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include "Clock.hpp"
#include "Texture.hpp"

namespace Game {

/** It's mandatory that directions are the first 4 elements */
enum : unsigned short {
	ANIM_UP    = 0,
	ANIM_RIGHT = 1,
	ANIM_DOWN  = 2,
	ANIM_LEFT  = 3,
	ANIM_DEATH = 4,
	ANIM_WIN   = 5,
	ANIM_HURT  = 6
};

/**
 * An Animated is a drawable object whose sprite has a certain
 * number of associated animations.
 */
class Animated : public Game::Component, public sf::Drawable {
protected:
	Game::Texture *texture;
	Game::Clock<1> *frameClock;
	std::unordered_map<std::string, Animation> animations;
	AnimatedSprite animatedSprite;

public:
	explicit Animated(Game::Entity *const owner, const std::string& texture_name);
	
	Animation& addAnimation(const std::string& name, bool set = false);
	Animation* getAnimation(const std::string& name) const;
	void setAnimation(const std::string& name);
	AnimatedSprite& getSprite() { return animatedSprite; }
	void setSmooth(bool s) { texture->getTexture()->setSmooth(s); }
	void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
