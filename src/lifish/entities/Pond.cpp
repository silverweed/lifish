#include "Pond.hpp"
#include "Collider.hpp"
#include "Lifed.hpp"
#include "core.hpp"
#include "Bonusable.hpp"

using lif::Pond;

Pond::Pond(const sf::Vector2f& pos, const sf::Vector2f& size, int dam,
		std::initializer_list<lif::c_layers::Layer> damaged)
	: lif::Entity(pos)
	, damage(dam)
{
	for (auto layer : damaged)
		damagedc_layers |= 1 << layer;

	addComponent(new lif::Collider(*this, [this] (lif::Collider& cld) {
		// on collision
		if (!((damagedc_layers >> cld.getLayer()) & 1)) return;

		auto lifed = cld.getOwner().get<lif::Lifed>();
		if (lifed == nullptr) return;

		auto bonusable = cld.getOwner().get<lif::Bonusable>();
		if (bonusable == nullptr || !bonusable->hasBonus(lif::BonusType::SHIELD))
			lifed->decLife(damage);
	}, lif::c_layers::DEFAULT, size));
}
