#pragma once

namespace lif {

/**
 * A CameraShakeRequest holds all the information needed to create a CameraShake, except its target.
 * This struct is used to create CameraShakes when the target is not accessible to the requester:
 * the requesting object stacks its request into some accessible list, which is then polled by
 * the request manager, which can create the actual Shakes with its render target.
 */
struct CameraShakeRequest {
	float xAmplitude,
	      xFrequency,
	      yAmplitude,
	      yFrequency;
	sf::Time duration;
	float fadeFactor;

	CameraShakeRequest(float xAmplitude, float xFrequency,
			float yAmplitude, float yFrequency,
			sf::Time duration,
			float fadeFactor)
		: xAmplitude(xAmplitude)
		, xFrequency(xFrequency)
		, yAmplitude(yAmplitude)
		, yFrequency(yFrequency)
		, duration(duration)
		, fadeFactor(fadeFactor)
	{}
};

}
