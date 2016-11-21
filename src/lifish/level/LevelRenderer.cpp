#include "LevelRenderer.hpp"
#include "LevelManager.hpp"
#include "Level.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "ZIndexed.hpp"
#include <map>
#include <vector>

using Game::LevelRenderer;

LevelRenderer::LevelRenderer(Game::LevelManager& owner)
	: owner(owner)
{}

void LevelRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	owner._mtxLock();
	const auto level = owner.getLevel();
	if (level == nullptr || !level->isInitialized()) return;

	target.draw(*level, states);
	owner._mtxUnlock();

	// Draw according to z-index
	std::map<int, std::vector<const Game::Drawable*>> toDraw;

	owner.entities._mtxLock();
	owner.entities.apply([&target, &toDraw] (const Game::Entity *e) {
		const auto d = e->get<Game::Drawable>();
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

	owner.entities._mtxUnlock();
}
