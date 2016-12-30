#include "AcidPond.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"

using Game::AcidPond;

AcidPond::AcidPond(const sf::Vector2f& pos, const sf::Vector2f& size)
	: Game::Pond(pos, size, 1, { Game::Layers::PLAYERS })
{
	auto sprite = addComponent(new Game::Sprite(*this,
				Game::getAsset("graphics", "acid_pond.png"),
				sf::IntRect(0, 0, size.x, size.y)));
	sprite->getTexture()->setRepeated(true);
	addComponent(new Game::Drawable(*this, *sprite));
}
