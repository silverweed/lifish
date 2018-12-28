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

	// in microseconds
	TimeType gameTime = 0;
	TimeType prevFrameTime = 0;
	TimeType realTime = 0;

public:
	explicit Time()
		: zeroTime(clock.now())
	{}

	void update() {
		const auto now = std::chrono::duration_cast<std::chrono::microseconds>(clock.now() - zeroTime).count();
		const auto delta = now - realTime;
		realTime = now;

		prevFrameTime = gameTime;
		gameTime += static_cast<TimeType>(delta * timeScale);
	}

	sf::Time getGameTime() const {
		return sf::microseconds(gameTime);
	}

	sf::Time getDelta() const {
		static constexpr TimeType MAX_FRAME_TIME = 1'000'000 / 30;

		auto deltaInUs = gameTime - prevFrameTime;
		if (deltaInUs > MAX_FRAME_TIME)
			deltaInUs = MAX_FRAME_TIME;

		return sf::microseconds(deltaInUs);
	}

	void addTime(sf::Time t) {
		gameTime += t.asMicroseconds();
	}

	void setTimeScale(float scale) {
		timeScale = scale;
	}

	float getTimeScale() const {
		return timeScale;
	}

	void setPaused(bool p) {
		if (p)
			clock.pause();
		else
			clock.resume();
	}
};

}
