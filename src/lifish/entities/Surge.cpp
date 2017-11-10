#include "Surge.hpp"
#include "Animated.hpp"
#include "Clock.hpp"
#include "Drawable.hpp"
#include "Player.hpp"
#include "Killable.hpp"
#include "Bonusable.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "core.hpp"
#include "DebugPainter.hpp"

using lif::Surge;

static const sf::Vector2f SIZE(1024, 64);

Surge::Surge(const sf::Vector2f& pos, const lif::Angle& rotPerSecond, const lif::LevelManager& lm)
	: lif::Entity(pos)
	, lm(lm)
	, rotPerSecond(rotPerSecond)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "surge.png"));
	animated->setDefaultFrameTime(sf::seconds(0.07));
	animated->addAnimation("surge", {
		sf::IntRect(0, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(0, 1 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(0, 2 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(0, 3 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(0, 4 * SIZE.y, SIZE.x, SIZE.y),
	}, true);
	animated->getSprite().setOrigin(SIZE.x * 0.5, SIZE.y * 0.5);
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TALL_ENTITIES);
	clock = addComponent<lif::Clock>(*this);
}

void Surge::update() {
	lif::Entity::update();
	
	animated->getSprite().rotate(rotPerSecond.asDegrees() * clock->restart().asSeconds());

	_checkCollision();
}

void Surge::_checkCollision() {
	for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player == nullptr || player->get<lif::Killable>()->isKilled()
				|| player->get<lif::Bonusable>()->hasBonus(lif::BonusType::SHIELD))
		{
			continue;
		}	
		auto pos = player->getPosition();
		const auto diff = lif::length(pos - position);
		// rotate the point to the inverse angle than the Surge
		const auto angle = -animated->getSprite().getRotation();
		pos = sf::Vector2f(position.x + diff * std::sin(lif::deg2rad(angle)),
				position.y + diff * std::cos(lif::deg2rad(angle)));
		const sf::FloatRect aabb(position.x - SIZE.x * 0.5, position.y - SIZE.y * 0.5, SIZE.x, SIZE.y);
		lif::debugPainter->addRectangleAt(sf::Vector2f(aabb.left, aabb.top), SIZE, sf::Color(255, 0, 0, 128));
		lif::debugPainter->addCircleAt(pos, 5, sf::Color::Blue);
		if (aabb.contains(pos)) {
			// deal damage
			std::cout << "DAMAGE!\n";
		}
	}
}
