#include "collision_functions.hpp"
#include "utils.hpp"
#include "Player.hpp"
#include "Killable.hpp"
#include "Scored.hpp"
#include "Explosion.hpp"
#include "Lifed.hpp"

#include <iostream>

using CollisionFunc = Game::Collider::CollisionFunc;

CollisionFunc Game::hurtByExplosions(Game::Entity& e, unsigned opts) {
	auto killable = e.get<Game::Killable>();
	if (killable == nullptr)
		throw std::invalid_argument("Entity given to `hurtByAdjacentExplosions` has no Killable!");
	auto lifed = e.get<Game::Lifed>();
	if (lifed == nullptr)
		throw std::invalid_argument("Entity given to `hurtByAdjacentExplosions` has no Lifed!");
	auto scored = e.get<Game::Scored>();

	return [e, killable, lifed, scored, opts] (Game::Collider& cld) {
		// Check valid hit
		if (cld.getLayer() != Game::Layers::EXPLOSIONS || killable->isKilled())
			return;

		auto& expl = static_cast<Game::Explosion&>(cld.getOwnerRW());
		if ((opts & CFO_TAKE_SINGLE_HIT) && expl.hasDamaged(e))
			return;

		// Check adjacent condition
		bool ignore = false;
		if ((opts & CFO_ONLY_ADJACENT)) {
			const auto etile = Game::tile(cld.getOwner().getPosition());
			const auto mtile = Game::tile(e.getPosition());
			ignore = Game::manhattanDistance(etile, mtile) != 1;
		}
		if (ignore)
			return;

		// Deal damage
		if (lifed->decLife(expl.getDamage()) <= 0)
			killable->kill();
		expl.dealDamageTo(e);
		
		// Give points (optional)
		if (scored == nullptr)
			return;

		const auto source = dynamic_cast<const Game::Player*>(expl.getSourceEntity());
		if (source != nullptr)
			scored->setTarget(source->getInfo().id);
	};
}
