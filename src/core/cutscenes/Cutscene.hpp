#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

namespace lif {

struct Cutscene {
	using Layer = std::tuple<std::string, unsigned>;
	using Subtitle = std::pair<std::string, sf::Time>;

	const sf::Vector2u windowSize;
	const std::vector<Layer> layers;
	const std::vector<Subtitle> subtitles;

	explicit Cutscene(const sf::Vector2u& windowSize,
			const std::vector<Layer>& layers,
			const std::vector<Subtitle>& subtitles)
		: windowSize(windowSize)
		, layers(layers)
		, subtitles(subtitles)
	{}
};

}
