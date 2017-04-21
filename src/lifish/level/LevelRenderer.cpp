#include "LevelRenderer.hpp"
#include "LevelManager.hpp"
#include "Level.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "LevelNumText.hpp"
#include "ZIndexed.hpp"
#include <map>
#include <vector>

using lif::LevelRenderer;

LevelRenderer::LevelRenderer(lif::LevelManager& owner)
	: owner(owner)
{}

void LevelRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	owner._mtxLock();
	const auto level = owner.getLevel();
	if (level == nullptr || !level->isInitialized()) return;

	// Draw the level background
	target.draw(level->getBackground(), states);
	owner._mtxUnlock();

	// Draw according to z-index
	std::map<int, std::vector<const lif::Drawable*>> toDraw;

	owner.entities.mtxLock();
	owner.entities.apply([&target, &toDraw] (const lif::Entity *e) {
		const auto d = e->get<lif::Drawable>();
		if (d != nullptr) {
			const auto zidx = e->get<lif::ZIndexed>();
			if (zidx != nullptr)
				toDraw[zidx->getZIndex()].emplace_back(d);
			else
				toDraw[0].emplace_back(d);
		}
	});

	for (const auto& pair : toDraw)
		for (const auto d : pair.second)
			target.draw(*d, states);

	owner.entities.mtxUnlock();

	// Draw the level border
	owner._mtxLock();
	target.draw(level->getBorder(), states);
	const auto levelnumtext = level->get<lif::LevelNumText>();
	if (levelnumtext != nullptr)
		target.draw(*levelnumtext, states);
	owner._mtxUnlock();
}
