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
			sf::RectangleShape rect(sf::Vector2f(cr.width, cr.height));
			rect.setPosition(cr.left, cr.top);
			rect.setOrigin(c.getOwner().getOrigin());
			rect.setFillColor(color);
			target.draw(rect);
		};

		for (const auto c : cls) {
			auto cc = dynamic_cast<Game::CompoundCollider*>(c);
			if (cc == nullptr) {
				draw_coll_rect(target, *c, c->isPhantom() 
						? COLLIDER_PHANTOM_COLOR
						: COLLIDER_REGULAR_COLOR);
			} else {
				draw_coll_rect(target, *cc, COLLIDER_COMPOUND_COLOR);
				draw_coll_rect(target, static_cast<Game::Collider>(*c), COLLIDER_PHANTOM_COLOR);
				for (const auto& cld : cc->getColliders())
					draw_coll_rect(target, cld, cld.isPhantom() 
							? COLLIDER_PHANTOM_COLOR
							: COLLIDER_REGULAR_COLOR);
			}
		}
	});
}

void DebugRenderer::drawSHCells(sf::RenderTarget& target, const Game::SHCollisionDetector& cd) {
	const unsigned s = cd.getSubdivisions();
	const float w = Game::LEVEL_WIDTH * Game::TILE_SIZE / s,
	            h = Game::LEVEL_HEIGHT * Game::TILE_SIZE / s;
	sf::RectangleShape rect(w, h);
	rect.setOutlineThickness(2);
	rect.setFillColor(sf::Color(72, 209, 204, 60));
	rect.setOutlineColor(sf::Color(0, 139, 139, 255));

	for (unsigned short i = 0; i < s; ++i)
		for (unsigned short j = 0; j < s; ++j) {
			rect.setPosition(sf::Vector2f(i * w, j * h));
			target.draw(rect);
		}
}
