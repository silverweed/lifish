#include "Animated.hpp"

using Game::Animated;

Animated::Animated(Game::Entity *const owner, const std::string& texture_name) 
	: Game::Component(owner)
{
	texture = addComponent(new Game::Texture(this, texture_name));
	clock = addComponent(new Game::Clock<1>(this));
}

Animation& Animated::addAnimation(const std::string& name) {
	return animations[name];
}

void Animated::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(animatedSprite, states);
}
