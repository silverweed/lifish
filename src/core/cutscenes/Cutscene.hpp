#pragma once

#include <tuple>
#include <initializer_list>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "WindowContext.hpp"
#include "EntityGroup.hpp"
#include "ShadedText.hpp"

namespace lif {

class Cutscene : public lif::WindowContext {
protected:
	using Layer = std::tuple<std::string, unsigned>;
	using Subtitle = std::pair<std::string, sf::Time>;

	const sf::Vector2u windowSize;

	lif::EntityGroup entities;
	std::vector<Subtitle> subtitles;
	int curSubtitleIdx = -1;
	lif::ShadedText curSubtitle;
	sf::Time curSubtitleTime;
	sf::Clock subtitleClock;


	void _nextSubtitle();
public:
	explicit Cutscene(const sf::Vector2u& windowSize,
			std::initializer_list<Layer> layers, std::initializer_list<Subtitle> subtitles);

	void update() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
