#include "LevelRenderer.hpp"
#include "Drawable.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "LevelNumText.hpp"
#include "Sprite.hpp"
#include "ZIndexed.hpp"
#include <unordered_map>
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
	std::unordered_map<int, std::vector<const lif::Drawable*>> toDraw;
	toDraw.reserve(owner.entities.size());

	int minZ = 0, maxZ = 0;
	owner.entities.mtxLock();
	owner.entities.apply([
#ifndef RELEASE
			drawSelectiveLayers = drawSelectiveLayers,
			layersToDraw = &layersToDraw,
#endif
			&toDraw, &minZ, &maxZ] (const lif::Entity& e) {
		const auto d = e.get<lif::Drawable>();
		if (d == nullptr)
			return;
		const auto zidx = e.get<lif::ZIndexed>();
#ifndef RELEASE
		if (zidx && drawSelectiveLayers && layersToDraw->find(zidx->getZIndex()) == layersToDraw->end())
			return;
#endif
		if (zidx != nullptr) {
			const auto z = zidx->getZIndex();
			if (z < minZ) minZ = z;
			if (z > maxZ) maxZ = z;
			toDraw[z].emplace_back(d);
		} else {
			toDraw[0].emplace_back(d);
		}
	});

	for (int i = std::max(0, minZ); i <= maxZ; ++i) {
		const auto it = toDraw.find(i);
		if (it == toDraw.end()) continue;
		for (const auto d : it->second)
			target.draw(*d, states);
	}

	owner.entities.mtxUnlock();

	// Draw the level border
	owner._mtxLock();
	target.draw(level->getBorder(), states);
	owner._mtxUnlock();

	// Draw entities above border
	owner.entities.mtxLock();
	for (int i = std::min(maxZ, -1); i >= minZ; --i) {
		const auto it = toDraw.find(i);
		if (it == toDraw.end()) continue;
		for (const auto d : it->second)
			target.draw(*d, states);
	}
	owner.entities.mtxUnlock();

	owner._mtxLock();
	const auto levelnumtext = level->get<lif::LevelNumText>();
	if (levelnumtext != nullptr)
		target.draw(*levelnumtext, states);
	owner._mtxUnlock();
}
