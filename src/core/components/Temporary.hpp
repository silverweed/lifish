#pragma once

#include "Killable.hpp"

namespace lif {

/**
 * A Temporary is a special Killable component which automatically checks
 * if it's "expired" and kills itself if so.
 */
class Temporary : public lif::Killable {
	std::function<bool()> expireCondition;

public:
	explicit Temporary(lif::Entity& owner, std::function<bool()> expireCondition);
	explicit Temporary(lif::Entity& owner, std::function<bool()> expireCondition, OnKillCallback onKill);
	explicit Temporary(lif::Entity& owner, std::function<bool()> expireCondition,
			OnKillCallback onKill, CheckKillCallback checkKill);

	void update() override;
};

}
