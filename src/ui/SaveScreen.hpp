#pragma once

#include "Screen.hpp"

namespace lif {

namespace ui {

class SaveScreen : public lif::ui::Screen {
public:
	explicit SaveScreen(const sf::RenderWindow& window, const sf::Vector2u& size);
};

}

}
