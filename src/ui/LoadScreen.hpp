#pragma once

#include "Screen.hpp"

namespace lif {

namespace ui {

class LoadScreen : public lif::ui::Screen {
public:
	explicit LoadScreen(const sf::RenderWindow& window, const sf::Vector2u& size);
};

}

}
