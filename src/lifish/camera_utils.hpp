#pragma once

#include <SFML/System/Time.hpp>

namespace lif {

void requestCameraShake(float xAmplitude, float xFrequency, float yAmplitude, float yFrequency,
		sf::Time duration, float fadeFactor);

}
