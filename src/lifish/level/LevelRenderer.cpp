#include "LevelRenderer.hpp"
#include "LevelManager.hpp"
#include "Sprite.hpp"
#include "ZIndexed.hpp"
#include <map>
#include <vector>

using Game::LevelRenderer;

LevelRenderer::LevelRenderer(Game::LevelManager& owner)
	: owner(owner)
{}

void LevelRenderer::setOrigin(const sf::Vector2f& o) {
	if (owner.level != nullptr)
		const_cast<Game::Level*>(owner.level)->setOrigin(o);
	owner.entities.setOrigin(o);
}

void LevelRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	const auto level = owner.level;
	if (level == nullptr) return;

	target.draw(*level, states);

	// Draw according to z-index
	std::map<int, std::vector<Game::Drawable*>> toDraw;

	owner.entities.apply([&target, &toDraw] (const Game::Entity *e) {
		auto d = e->get<Game::Drawable>();
		if (d != nullptr) {
			auto zidx = e->get<Game::ZIndexed>();
			if (zidx != nullptr)
				toDraw[zidx->getZIndex()].push_back(d);
			else
				toDraw[0].push_back(d);
		}
	});

	for (const auto& pair : toDraw)
		for (const auto d : pair.second)
			target.draw(*d, states);
}
