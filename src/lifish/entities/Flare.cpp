#include "Flare.hpp"
#include "LightSource.hpp"
#include "Clock.hpp"
#include "Temporary.hpp"

using lif::Flare;

Flare::Flare(sf::Time burstTime, sf::Time decayTime)
	: lif::Entity({ 0, 0 })
	, burstTime(burstTime)
	, decayTime(decayTime)
{
	light = addComponent<lif::LightSource>(*this, 10000, sf::Color::Black);
	lightClock = addComponent<lif::Clock>(*this);
	addComponent<lif::Temporary>(*this, [this] () {
		return !bursting && lightClock->getElapsedTime() >= this->decayTime;
	});
}

void Flare::update() {
	lif::Entity::update();

	if (bursting) {
		const auto white = std::min(255,
			static_cast<int>(255 * static_cast<float>(lightClock->getElapsedTime() / burstTime)));
		light->setColor(sf::Color(white, white, white));
		if (lightClock->getElapsedTime() >= burstTime) {
			bursting = false;
			lightClock->restart();
		}
	} else {
		const auto white = std::max(0,
			static_cast<int>(255 * (1.f - lightClock->getElapsedTime() / decayTime)));
		light->setColor(sf::Color(white, white, white));
	}
}
