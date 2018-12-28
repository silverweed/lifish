#include "Flare.hpp"
#include "LightSource.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "core.hpp"

using lif::Flare;

Flare::Flare(sf::Time burstTime, sf::Time decayTime)
	: lif::Entity({ 0, 0 })
	, burstTime(burstTime)
	, decayTime(decayTime)
{
	light = addComponent<lif::LightSource>(*this, 10000, sf::Color::Black);
	addComponent<lif::Temporary>(*this, [this] () {
		return !bursting && lightT >= this->decayTime;
	});
}

void Flare::update() {
	lif::Entity::update();

	lightT += lif::time.getDelta();

	if (bursting) {
		const auto white = std::min(255,
			static_cast<int>(255 * static_cast<float>(lightT / burstTime)));
		light->setColor(sf::Color(white, white, white));
		if (lightT >= burstTime) {
			bursting = false;
			lightT = sf::Time::Zero;
		}
	} else {
		const auto white = std::max(0,
			static_cast<int>(255 * (1.f - lightT / decayTime)));
		light->setColor(sf::Color(white, white, white));
	}
}
