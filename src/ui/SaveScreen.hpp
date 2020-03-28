#pragma once

#include <array>
#include "DynamicScreen.hpp"

namespace sf {

class RectangleShape;

}

namespace lif {

class ShadedText;

namespace ui {

class SaveScreen : public lif::ui::DynamicScreen {
public:
	static constexpr int MAX_SAVE_FILE_NAME_LEN = 10;

private:
	std::array<char, MAX_SAVE_FILE_NAME_LEN> buffer;
	size_t bufIdx = 0;

	sf::RectangleShape *cursor = nullptr;
	sf::Clock cursorClock;
	bool cursorVisible = true;
	sf::Vector2f charBounds;

	lif::ShadedText *bufferText = nullptr;
	lif::ShadedText *errText = nullptr;

	void _updateCursorPosition();

	void build() override;
public:
	static constexpr const char *SCREEN_NAME = "save";

	explicit SaveScreen(const sf::RenderWindow& window, const sf::Vector2u& size);

	std::string getPrompt() const;

	bool handleEvent(sf::Window&, sf::Event) override;
	void update() override;
	void onLoad() override;
};

}

}
