#include "GuidedBullet.hpp"
#include "game.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "FreeMoving.hpp"

using lif::GuidedBullet;

GuidedBullet::GuidedBullet(const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
			const lif::BulletInfo& info,
			const lif::Entity *const source,
			std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs)
	: lif::FreeBullet(start, lif::radians(0), info, source)
{
	addComponent<lif::GuidedMoving>(*this, start, end, timeTaken, modfuncs);
	get<lif::FreeMoving>()->setActive(false);
}
