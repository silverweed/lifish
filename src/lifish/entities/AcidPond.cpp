#include "AcidPond.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"

using lif::AcidPond;

AcidPond::AcidPond(const sf::Vector2f& pos, const sf::Vector2f& size)
	: lif::Pond(pos, size, 1, { lif::Layers::PLAYERS })
{
	auto sprite = addComponent(new lif::Sprite(*this,
				lif::getAsset("graphics", "acid_pond.png"),
				sf::IntRect(0, 0, size.x, size.y)));
	sprite->getTexture()->setRepeated(true);
	addComponent(new lif::Drawable(*this, *sprite));
}
