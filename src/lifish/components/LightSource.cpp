#include "LightSource.hpp"
#include "core.hpp"
#include <random>

using lif::LightSource;

LightSource::LightSource(lif::Entity& owner, float radius, sf::Color color, float flickerIntensity,
		unsigned short flickerLen)
	: lif::Component(owner)
	, radius(radius)
	, color(color)
	, flickerLen(flickerLen)
	, smoothing(flickerLen)
{
	if (flickerIntensity > 0) {
		_fillRandomPool(flickerIntensity);
		for (unsigned i = 0; i < flickerLen; ++i) 
			_flickerStep();
	}
}

void LightSource::update() {
	lif::Component::update();

	if (flickerLen > 0)
		color.a = 256 * _flickerStep();
}

void LightSource::_fillRandomPool(float flickerAmount) {
	std::uniform_real_distribution<float> dist(0, 1);
	for (unsigned i = 0; i < randomPool.size(); ++i) {
		randomPool[i] = (1 - flickerAmount) + flickerAmount * dist(lif::rng);
	}
}

float LightSource::_flickerStep() {
	const unsigned len = smoothing.size();
	static int j = 0;
	float sum = 0;
	for (unsigned i = 1; i < len; ++i) {
		smoothing[i-1] = smoothing[i];
		sum += smoothing[i-1];
	}
	smoothing[len-1] = randomPool[j];
	j = (j + 1) % randomPool.size();
	sum += smoothing[len-1];
	return sum / len;
}
