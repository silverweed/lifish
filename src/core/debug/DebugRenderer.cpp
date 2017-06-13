#include "DebugRenderer.hpp"
#include "SHCollisionDetector.hpp"
#include "EntityGroup.hpp"

#define COLLIDER_REGULAR_COLOR sf::Color(255, 0, 255, 110)
#define COLLIDER_PHANTOM_COLOR sf::Color(194, 194, 194, 110)
#define COLLIDER_INACTIVE_COLOR sf::Color(194, 194, 194, 92)

using lif::debug::DebugRenderer;

void DebugRenderer::drawColliders(sf::RenderTarget& target, const lif::EntityGroup& group) {
	group.apply([&target] (const lif::Entity *e) {
		auto cls = e->getAllRecursive<lif::Collider>();

		const auto draw_coll_rect = [] (sf::RenderTarget& target, const lif::Collider& c, sf::Color color) {
			const auto cr = c.getRect();
			sf::RectangleShape rect(sf::Vector2f(cr.width - 1, cr.height - 1));
			rect.setPosition(cr.left, cr.top);
			rect.setOrigin(c.getOwner().getOrigin());
			rect.setFillColor(color);
			rect.setOutlineColor(sf::Color(color.r + 40, color.g + 40, color.b + 40, 240));
			rect.setOutlineThickness(1);
			target.draw(rect);
		};

		for (const auto c : cls) {
			draw_coll_rect(target, *c, c->isActive()
					? c->isPhantom()
						? COLLIDER_PHANTOM_COLOR
						: COLLIDER_REGULAR_COLOR
					: COLLIDER_INACTIVE_COLOR);
		}
	});
}

void DebugRenderer::drawSHCells(sf::RenderTarget& target, const lif::SHCollisionDetector& cd) {
	const unsigned s = cd.getSubdivisions();
	const auto limit = cd.getLevelLimit();
	const float w = static_cast<float>(limit.width - limit.left) / s,
	            h = static_cast<float>(limit.height - limit.top) / s;
	sf::RectangleShape rect(sf::Vector2f(w, h));
	rect.setOutlineThickness(2);
	rect.setFillColor(sf::Color(72, 209, 204, 60));
	rect.setOutlineColor(sf::Color(0, 139, 139, 255));

	for (unsigned i = 0; i < s; ++i) {
		for (unsigned j = 0; j < s; ++j) {
			rect.setPosition(i * w + lif::TILE_SIZE, j * h + lif::TILE_SIZE);
			target.draw(rect);
		}
	}

	// Draw level limit
	sf::RectangleShape limitRect(sf::Vector2f(limit.width - limit.left, limit.height - limit.top));
	limitRect.setPosition(limit.left, limit.top);
	limitRect.setOutlineColor(sf::Color(255, 100, 100, 255));
	limitRect.setFillColor(sf::Color(0, 0, 0, 0));
	limitRect.setOutlineThickness(4);
	target.draw(limitRect);
}
