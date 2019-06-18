#pragma once

#include <SFML/System/Time.hpp>
#include <chrono>

namespace lif {

template<typename ClockType>
class Time final {

	using TimeType = unsigned long long;

	const std::chrono::time_point<ClockType> zeroTime;

	ClockType clock;
	float timeScale = 1;
	float prevTimeScale = 1;

	// in microseconds
	TimeType gameTime = 0;
	TimeType prevFrameTime = 0;
	TimeType realTime = 0;
	TimeType prevRealTime = 0;

	bool skipFrameLock = false;

public:
	explicit Time()
		: zeroTime(clock.now())
	{}

	void update() {
		const auto now = std::chrono::duration_cast<std::chrono::microseconds>(clock.now() - zeroTime).count();
		const auto delta = now - realTime;
		prevRealTime = realTime;
		realTime = now;

		prevFrameTime = gameTime;
		gameTime += static_cast<TimeType>(delta * static_cast<double>(timeScale));

		skipFrameLock = false;
	}

	sf::Time getGameTime() const {
		return sf::microseconds(gameTime);
	}

	sf::Time getDelta() const {
		static constexpr TimeType MAX_FRAME_TIME = 1'000'000 / 30;

		auto deltaInUs = gameTime - prevFrameTime;
		if (!skipFrameLock && deltaInUs > MAX_FRAME_TIME * timeScale)
			deltaInUs = MAX_FRAME_TIME * static_cast<double>(timeScale);

		return sf::microseconds(deltaInUs);
	}

	sf::Time getRealDelta() const {
		return sf::microseconds(realTime - prevRealTime);
	}

	void addTime(sf::Time t) {
		prevFrameTime = gameTime;
		gameTime += t.asMicroseconds();
		skipFrameLock = true;
	}

	void setTimeScale(float scale) {
		timeScale = scale;
	}

	float getTimeScale() const {
		return timeScale;
	}

	void pause() {
		if (timeScale != 0)
			prevTimeScale = timeScale;
		timeScale = 0;
	}

	void resume() {
		timeScale = prevTimeScale;
	}
};

}
