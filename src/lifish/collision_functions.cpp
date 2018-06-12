#include "collision_functions.hpp"
#include "Bonusable.hpp"
#include "Explosion.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "utils.hpp"

using CollisionFunc = lif::Collider::CollisionFunc;

CollisionFunc lif::hurtByExplosions(lif::Entity& e, unsigned opts) {
	auto killable = e.get<lif::Killable>();
	if (killable == nullptr)
		throw std::invalid_argument("Entity given to `hurtByExplosions` has no Killable!");
	auto lifed = e.get<lif::Lifed>();
	if (lifed == nullptr)
		throw std::invalid_argument("Entity given to `hurtByExplosions` has no Lifed!");
	auto scored = e.get<lif::Scored>();
	auto bonusable = e.get<lif::Bonusable>();

	return [&e, killable, lifed, scored, bonusable, opts] (lif::Collider& cld) {
		// Check valid hit
		if (cld.getLayer() != lif::c_layers::EXPLOSIONS || killable->isKilled())
			return;

		// Check shield
		if (bonusable != nullptr && bonusable->hasBonus(lif::BonusType::SHIELD))
			return;

		auto& expl = static_cast<lif::Explosion&>(cld.getOwnerRW());
		if (expl.hasDamaged(e))
			return;

		// Check adjacent condition
		bool ignore = false;
		if ((opts & CFO_ONLY_ADJACENT)) {
			const auto etile = lif::tile(cld.getOwner().getPosition());
			const auto mtile = lif::tile(e.getPosition());
			ignore = lif::manhattanDistance(etile, mtile) != 1;
		}
		if (ignore)
			return;

		// Deal damage
		auto dmg = expl.getDamage();
		if ((opts & CFO_1_DMG_PER_HIT))
			dmg = std::min(1, dmg);

		if (lifed->decLife(dmg) <= 0)
			killable->kill();
		expl.dealDamageTo(e);

		// Give points (optional)
		if (scored == nullptr)
			return;

		const auto source = dynamic_cast<const lif::Player*>(expl.getSourceEntity());
		if (source != nullptr)
			scored->setTarget(source->getInfo().id);
	};
}
