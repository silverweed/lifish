#include "CameraShake.hpp"
#include "CameraShakeRequest.hpp"
#include "Clock.hpp"
#include "Temporary.hpp"
#include <cmath>
#include <random>
#include "core.hpp"

using lif::CameraShake;

CameraShake::CameraShake(sf::RenderTarget& target,
			float xAmplitude, float xFrequency,
			float yAmplitude, float yFrequency,
			sf::Time duration,
			float fadeFactor)
	: lif::Entity({ 0, 0 })
	, target(target)
	, offX(std::uniform_real_distribution<float>(0, 2 * lif::PI)(lif::rng))
	, offY(std::uniform_real_distribution<float>(0, 2 * lif::PI)(lif::rng))
	, xAmplitude(xAmplitude)
	, xFrequency(xFrequency)
	, yAmplitude(yAmplitude)
	, yFrequency(yFrequency)
	, fadeFactor(fadeFactor)
{
	shakeClock = addComponent<lif::Clock>(*this);
	expireClock = addComponent<lif::Clock>(*this);
	addComponent<lif::Temporary>(*this, [this, duration] () {
		return expireClock->getElapsedTime() > duration;
	}, [this] () {
		// on kill
		auto view = this->target.getView();
		view.setViewport({ 0, 0, 1, 1 });
		this->target.setView(view);
		stopUpdating = true;
	});
}

CameraShake::CameraShake(sf::RenderTarget& target, const lif::CameraShakeRequest& req)
	: CameraShake(target, req.xAmplitude, req.xFrequency, req.yAmplitude, req.yFrequency,
			req.duration, req.fadeFactor)
{}

void CameraShake::update() {
	lif::Entity::update();

	if (stopUpdating) return;

	auto view = target.getView();
	const auto t = shakeClock->getElapsedTime().asSeconds();
	const auto fade = std::max(0.1f, std::pow(t + 1, fadeFactor));
	auto newX = xAmplitude * std::sin(t * xFrequency + offX) / fade;
	auto newY = yAmplitude * std::sin(t * yFrequency + offY) / fade;

	view.setViewport({ newX, newY, 1.f, 1.f });
	target.setView(view);
}
