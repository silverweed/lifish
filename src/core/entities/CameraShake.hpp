#pragma once

#include "Entity.hpp"
#include <SFML/Graphics.hpp>

namespace lif {

struct CameraShakeRequest;

class CameraShake : public lif::Entity {

	sf::RenderTarget& target;

	// Random initial offsets
	const float offX, offY;
	const float xAmplitude,
	            xFrequency,
	            yAmplitude,
	            yFrequency,
	            fadeFactor;
	bool stopUpdating = false;

	sf::Time shakeT = sf::Time::Zero,
	         expireT = sf::Time::Zero;
public:
	/** Constructs a CameraShake for `target`. Amplitudes are relative, so they should range from 0 to 1 inclusive.
	 *  Frequencies are in hertz. The shake is modulated by a 1/t^e factor, where e = `fadeFactor`.
	 */
	explicit CameraShake(sf::RenderTarget& target,
			float xAmplitude, float xFrequency,
			float yAmplitude, float yFrequency,
			sf::Time duration,
			float fadeFactor = 1);
	/** Constructs a CameraShake from a CameraShakeRequest, with the given target.
	 *  This constructor is used by managing systems that poll CameraShakeRequests from
	 *  their managed objects and create the concrete CameraShakes with their own RenderTarget.
	 */
	explicit CameraShake(sf::RenderTarget& target, const lif::CameraShakeRequest& req);

	void update() override;
};

}
