#include "DebugRenderer.hpp"
#include "DebugPainter.hpp"
#include "EntityGroup.hpp"
#include "SHCollisionDetector.hpp"
#include <sstream>

#define COLLIDER_REGULAR_COLOR sf::Color(255, 0, 255, 110)
#define COLLIDER_COLLIDING_COLOR sf::Color(255, 0, 5, 180)
#define COLLIDER_PHANTOM_COLOR sf::Color(194, 194, 194, 110)
#define COLLIDER_INACTIVE_COLOR sf::Color(194, 194, 194, 92)

using lif::debug::DebugRenderer;

void DebugRenderer::drawColliders(const lif::EntityGroup& group) {
	group.apply([] (const lif::Entity& e) {
		for (const auto c : e.getAllRecursive<lif::Collider>()) {
			const auto color = c->isActive()
						? c->isPhantom()
							? COLLIDER_PHANTOM_COLOR
							: c->getColliding().size() > 0
								? COLLIDER_COLLIDING_COLOR
								: COLLIDER_REGULAR_COLOR
						: COLLIDER_INACTIVE_COLOR;
			lif::debugPainter->addRectangleAt(
				c->getPosition(), sf::Vector2f(c->getRect().width, c->getRect().height),
				color,
				1, sf::Color(color.r + 40, color.g + 40, color.b + 40, 240));
			std::stringstream ss;
			const auto& pos = c->getPosition();
			const auto p = sf::Vector2i(pos);
			const auto t = lif::tile(pos);
			const auto t2 = lif::tile2(pos);
			ss << p.x << "," << p.y << "\n " << t.x << "," << t.y << "\n " << t2.x << "," << t2.y;
			lif::debugPainter->addTextAt(c->getPosition() + sf::Vector2f(1, 1), ss.str(), 6,
					sf::Color(140, 140, 140));
			if (c->getOwner().isAligned())
				lif::debugPainter->addTextAt(c->getPosition() + c->getSize() - sf::Vector2f(7, 9),
					"A", 6, sf::Color(140, 140, 140));
		}
	});
}

void DebugRenderer::drawSHCells(const lif::SHCollisionDetector& cd) {
	const unsigned s = cd.getSubdivisions();
	const auto limit = cd.getLevelLimit();
	const float w = static_cast<float>(limit.width - limit.left) / s,
	            h = static_cast<float>(limit.height - limit.top) / s;
	const auto color = sf::Color(72, 209, 204, 60);
	const auto outlineColor = sf::Color(0, 139, 139, 255);

	for (unsigned i = 0; i < s; ++i) {
		for (unsigned j = 0; j < s; ++j) {
			lif::debugPainter->addRectangleAt(
				sf::Vector2f(i * w + lif::TILE_SIZE, j * h + lif::TILE_SIZE),
				sf::Vector2f(w, h), color, 2, outlineColor);
		}
	}

	// Draw level limit
	lif::debugPainter->addRectangleAt(sf::Vector2f(limit.left, limit.top),
			sf::Vector2f(limit.width - limit.left, limit.height - limit.top),
			sf::Color(0, 0, 0, 0), 4, sf::Color(255, 100, 100, 255));
}
