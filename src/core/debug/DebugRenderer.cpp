#include "DebugRenderer.hpp"
#include "CompoundCollider.hpp"

#define COLLIDER_REGULAR_COLOR sf::Color(255, 0, 255, 110)
#define COLLIDER_PHANTOM_COLOR sf::Color(194, 194, 194, 110)
#define COLLIDER_COMPOUND_COLOR sf::Color(194, 194, 194, 92)

using Debug::DebugRenderer;

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
