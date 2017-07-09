#include "camera_utils.hpp"
#include "GlobalDataPipe.hpp"
#include "CameraShakeRequest.hpp"

void lif::requestCameraShake(float xAmplitude, float xFrequency, float yAmplitude, float yFrequency,
		sf::Time duration, float fadeFactor)
{
	lif::GlobalDataPipe<lif::CameraShakeRequest>::getInstance().add(xAmplitude, xFrequency,
			yAmplitude, yFrequency, duration, fadeFactor);
}
