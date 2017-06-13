#include "EnemyExplosion.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "collision_layers.hpp"

using lif::EnemyExplosion;

EnemyExplosion::EnemyExplosion(const sf::Vector2f& pos, unsigned short _radius, int damage,
		const lif::Entity *const source, bool isIncendiary)
	: lif::Explosion(pos, _radius, source, isIncendiary, damage, lif::c_layers::ENEMY_EXPLOSIONS)
{
	explosionV->getSprite().setColor(sf::Color::Green);
	explosionH->getSprite().setColor(sf::Color::Green);
	explosionC->getSprite().setColor(sf::Color::Green);
}
