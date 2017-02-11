#include "AcidPond.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"

using lif::AcidPond;

AcidPond::AcidPond(const sf::Vector2f& pos, const sf::Vector2f& size)
	: lif::Pond(pos, size, 1, { lif::c_layers::PLAYERS })
{
	auto sprite = addComponent(std::make_shared<lif::Sprite>(*this,
				lif::getAsset("graphics", "acid_pond.png"),
				sf::IntRect(0, 0, size.x, size.y)));
	sprite->getTexture()->setRepeated(true);
	addComponent(std::make_shared<lif::Drawable>(*this, *sprite));
}
