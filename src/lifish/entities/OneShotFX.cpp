#include "OneShotFX.hpp"
#include "game.hpp"
#include "Drawable.hpp"
#include "Temporary.hpp"
#include "Animated.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"

using lif::OneShotFX;

OneShotFX::OneShotFX(const sf::Vector2f& pos, const std::string& texture, std::initializer_list<sf::IntRect> frames)
	: lif::Entity(pos)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", texture));
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::FLASHES);

	animated->addAnimation("fx", frames, true);

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();

	addComponent<lif::Temporary>(*this, [&animatedSprite] () {
		return !animatedSprite.isPlaying();
	});
}
