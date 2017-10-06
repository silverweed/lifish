#include "CutsceneBuilder.hpp"
#include "Options.hpp"
#include "json.hpp"
#include <fstream>
#include <utility>
#include "core.hpp"

using lif::CutsceneBuilder;
using json = nlohmann::json;

std::vector<lif::Cutscene> CutsceneBuilder::fromJson(const std::string& cutsceneName) {
	std::vector<lif::Cutscene> cutscenes;

	json cutsceneJSON = json::parse(std::ifstream(lif::getAsset("cutscenes", cutsceneName + ".json")));

	for (const auto& scene : cutsceneJSON["scenes"]) {
		std::vector<lif::Cutscene::Layer> layers;
		std::vector<lif::Cutscene::Subtitle> subs;
		for (const auto& layer : scene["layers"])
			layers.emplace_back(lif::getAsset("cutscenes", layer["img"].get<std::string>()),
						layer["z"].get<unsigned>());
		for (const auto& sub : scene["subs"])
			subs.emplace_back(sub["s"].get<std::string>(), sf::seconds(sub["duration"].get<float>()));

		cutscenes.emplace_back(lif::options.windowSize, layers, subs);
	}

	return cutscenes;
}
