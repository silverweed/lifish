#pragma once

#include <SFML/Graphics.hpp>
#ifndef RELEASE
#	include <unordered_set>
#endif

namespace lif {

class LevelManager;

class LevelRenderer final : public sf::Drawable {
	lif::LevelManager& owner;

#ifndef RELEASE
	std::unordered_set<int> layersToDraw;
	bool drawSelectiveLayers = false;
#endif

public:
	explicit LevelRenderer(lif::LevelManager& owner);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

#ifndef RELEASE
	void setDrawSelectiveLayers(bool d) {
		drawSelectiveLayers = d;
	}

	void resetDrawnLayers() {
		layersToDraw.clear();
	}

	const std::unordered_set<int>& addDrawnLayer(int l) {
		layersToDraw.insert(l);
		return layersToDraw;
	}

	bool isSelectiveLayerDrawingEnabled() const { return drawSelectiveLayers; }
#endif
};

}
