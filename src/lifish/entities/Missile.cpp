#include "Missile.hpp"
#include "Killable.hpp"
#include "core.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "Targeter.hpp"
#include "Temporary.hpp"
#include "Spawning.hpp"
#include <tuple>
#include <random>

using lif::Missile;

Missile::Missile(const sf::Vector2f& pos, const sf::Vector2f& target,
		const lif::BulletInfo& info, const lif::Entity *const source)
	: lif::GuidedBullet(pos, target, info, sf::seconds(1.0 / info.speed), source)
{
	collider->setActive(false); // collide with nothing, not even level bounds

	// randomize the wave function a bit
	std::uniform_real_distribution<float> dist(6, 20);
	const auto ampl = dist(lif::rng) * lif::TILE_SIZE;
	const auto freq = dist(lif::rng);
	const auto wave = [ampl, freq] (auto t) {
		const auto y = ampl * (t - t * t);
		const auto x = 0.5 * lif::TILE_SIZE * std::sin(freq * t * lif::PI);
		return sf::Vector2f(x, -y);
	};
	static_cast<lif::GuidedMoving*>(moving)->addModFunc(std::make_tuple(wave, false, false));

	// spawn/despawn the targeter
	addComponent<lif::Spawning>(*this, [] (const lif::Spawning& spw) {
		// spawn condition
		return spw.nSpawned() < 1;
	}, [this, target] () {
		// spawn function
		auto targeter = new lif::Targeter(target);
		auto klb = get<lif::Killable>();
		targeter->addComponent<lif::Temporary>(*targeter, [this, klb] () {
			// expire condition
			return klb->isKilled();
		});
		return targeter;
	});

	// TODO: rotate the sprite properly
	//auto animated = get<lif::Animated>();
	//animated->getSprite().setOrigin(data.size / 2, data.size / 2);
}

void Missile::update() {
	lif::Entity::update();

	if (static_cast<const lif::GuidedMoving*>(moving)->getPerc() >= 1) {
		collider->setActive(true);
		get<lif::Killable>()->kill();
	}
}
