#include "Pond.hpp"
#include "Collider.hpp"
#include "Lifed.hpp"
#include "core.hpp"
#include "Bonusable.hpp"

using Game::Pond;

Pond::Pond(const sf::Vector2f& pos, const sf::Vector2f& size, int dam,
		std::initializer_list<Game::Layers::Layer> damaged)
	: Game::Entity(pos)
	, damage(dam)
{
	for (auto layer : damaged)
		damagedLayers |= 1 << layer;

	addComponent(new Game::Collider(*this, [this] (Game::Collider& cld) {
		// on collision
		if (!((damagedLayers >> cld.getLayer()) & 1)) return;

		auto lifed = cld.getOwner().get<Game::Lifed>();
		if (lifed == nullptr) return;

		auto bonusable = cld.getOwner().get<Game::Bonusable>();
		if (bonusable == nullptr || !bonusable->hasBonus(Game::BonusType::SHIELD))
			lifed->decLife(damage);
	}, Game::Layers::DEFAULT, size));
}
