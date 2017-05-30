#pragma once

#include <functional>
#include <initializer_list>
#include <tuple>
#include "Moving.hpp"

namespace lif {

class Clock;

/**
 * A GuidedMoving has a starting and an ending position, and will travel from the first
 * to the second one in the given time.
 * It may optionally have one or more "path modifier functions" which alter the way it travels from point
 * `start` to `end`.
 * Such functions have the signature double -> sf::Vector2f, where the input `double` is the percentage of
 * the original path travelled so far, and the output `sf::Vector2f` is the offset to be added to that path.
 * Note that the x and y of such offset are _relative to the path_, so an offset of (0, 1) shifts the
 * bullet of 1 unit _orthogonally_ to the line linking `start` to `end`.
 */
class GuidedMoving : public lif::Moving {
	using _ModFunc = std::function<sf::Vector2f(double)>;
public:
	/** { function, whether to accept inputs > 1 or not } */
	using ModFunc = std::pair<_ModFunc, bool>;

private:
	sf::Vector2f _calcPathPos(float perc) const;
	sf::Vector2f _calcModFunc(const _ModFunc& f, float perc) const;

protected:
	const sf::Vector2f start;
	const sf::Vector2f end;
	const sf::Time timeTaken;
	std::vector<ModFunc> modfuncs;

	lif::Clock *clock = nullptr;

public:
	explicit GuidedMoving(lif::Entity& owner,
			const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
			std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs = {});

	bool isAtEnd() const;

	void reset();

	void update() override;
};

}
