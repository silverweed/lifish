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
 * Such functions have the signature float -> sf::Vector2f, where the input `float` is the percentage of
 * the original path travelled so far, and the output `sf::Vector2f` is the offset to be added to that path.
 * Note that the x and y of such offset can be either _relative to the path_ (so an offset of (0, 1) shifts the
 * bullet of 1 unit orthogonally to the line linking `start` to `end`), or _absolute_, depending on the 3rd
 * argument of the tuple given as a ModFunc.
 */
class GuidedMoving : public lif::Moving {
	using _ModFunc = std::function<sf::Vector2f(float)>;
public:
	/** { function, accept inputs > 1, use relative coordinates } */
	using ModFunc = std::tuple<_ModFunc, bool, bool>;

private:
	sf::Vector2f _calcPathPos(float perc) const;
	sf::Vector2f _calcModFunc(const _ModFunc& f, float perc, bool useRelative) const;
	void _updatePosition();

protected:
	const sf::Vector2f start;
	const sf::Vector2f end;
	const sf::Time timeTaken;
	std::vector<ModFunc> modfuncs;
	float tPerc;

	lif::Clock *clock = nullptr;

public:
	explicit GuidedMoving(lif::Entity& owner,
			const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
			std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs = {});

	/** This constructor enables forwarding of the modfunc parameter from `addComponent` and similar.
	 *  Since it's a pretty common case to have only one modfunc, it serves the practical purpose
	 *  of adding a component without ugly workarounds.
	 */
	explicit GuidedMoving(lif::Entity& owner,
			const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
			lif::GuidedMoving::ModFunc modfunc);

	bool isAtEnd() const;

	void reset();

	void update() override;

	/** @return The current time, in percentage [0-inf) */
	float getPerc() const { return tPerc; }
};

}
