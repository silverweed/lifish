#pragma once

#include "DynamicScreen.hpp"

namespace lif {

namespace ui {

class HighScoreScreen : public lif::ui::DynamicScreen {
	unsigned nonInteractableSizeExcludingHighScores = 0;

	void build() override;

public:
	static constexpr const char *SCREEN_NAME = "high_score";

	explicit HighScoreScreen(const sf::RenderWindow& window, const sf::Vector2u& size);

	void onLoad() override;
};

} // end namespace ui

} // end namespace lif
