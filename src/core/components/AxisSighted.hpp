#pragma once

#include "Sighted.hpp"
#include "Direction.hpp"

namespace lif {

/** An AxisSighted entity has knowledge of entities around it along axis directions. */
class AxisSighted : public lif::Sighted {
	using PartSeenEntitiesList = std::vector<std::pair<std::weak_ptr<lif::Entity>, unsigned short>>;
	using TotSeenEntitiesList = std::array<
		PartSeenEntitiesList,
		static_cast<unsigned short>(lif::Direction::NONE)>;

	/** all entities seen in each direction, and their distance (nearest first) */
	TotSeenEntitiesList seen;

	/** Fills seen[dir] with entities seen in that direction */
	void _fillLine(const lif::Direction dir);

public:
	explicit AxisSighted(lif::Entity& owner, float visionRadius = -1);

	/** @return A list of pair {entity, distance_in_tiles (discrete)} as seen in all directions */
	const TotSeenEntitiesList& entitiesSeen() const { return seen; }

	/** @return A list of pair {entity, distance_in_tiles (discrete)} as seen along direction `dir` */
	const PartSeenEntitiesList& entitiesSeen(lif::Direction dir) const { 
		return seen[static_cast<size_t>(dir)];
	}
	
	void update() override;
};

}
