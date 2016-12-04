#include "DebugRenderer.hpp"
#include "CompoundCollider.hpp"
#include "SHCollisionDetector.hpp"
#include "EntityGroup.hpp"
#include <SFML/Graphics.hpp>

#define COLLIDER_REGULAR_COLOR sf::Color(255, 0, 255, 110)
#define COLLIDER_PHANTOM_COLOR sf::Color(194, 194, 194, 110)
#define COLLIDER_COMPOUND_COLOR sf::Color(194, 194, 194, 92)

using Game::Debug::DebugRenderer;

void DebugRenderer::drawColliders(sf::RenderTarget& target, const Game::EntityGroup& group) {
	group.apply([&target] (const Game::Entity *e) {
		auto cls = e->getAllRecursive<Game::Collider>();

		auto draw_coll_rect = [] (sf::RenderTarget& target, const Game::Collider& c, sf::Color color) {
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
			draw_coll_rect(target, *c, c->isPhantom()
					? COLLIDER_PHANTOM_COLOR
					: COLLIDER_REGULAR_COLOR);
		}
	});
}

void DebugRenderer::drawSHCells(sf::RenderTarget& target, const Game::SHCollisionDetector& cd) {
	const unsigned s = cd.getSubdivisions();
	const float w = float(Game::LEVEL_WIDTH * Game::TILE_SIZE) / s,
	            h = float(Game::LEVEL_HEIGHT * Game::TILE_SIZE) / s;
	sf::RectangleShape rect(sf::Vector2f(w, h));
	rect.setOutlineThickness(2);
	rect.setFillColor(sf::Color(72, 209, 204, 60));
	rect.setOutlineColor(sf::Color(0, 139, 139, 255));

	for (unsigned short i = 0; i < s; ++i)
		for (unsigned short j = 0; j < s; ++j) {
			rect.setPosition(sf::Vector2f(i * w + Game::SIDE_PANEL_WIDTH + Game::TILE_SIZE,
						j * h + Game::TILE_SIZE));
			target.draw(rect);
		}
}
