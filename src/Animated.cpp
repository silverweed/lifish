#include "Animated.hpp"

using Game::Animated;

void Animated::draw(sf::RenderTarget& window) {
	window.draw(animatedSprite);
}
