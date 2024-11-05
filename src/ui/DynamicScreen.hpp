#pragma once

#include "Screen.hpp"

namespace lif {
namespace ui {
class DynamicScreen : public lif::ui::Screen {
protected:
	virtual void build() = 0;

public:
	explicit DynamicScreen(const sf::RenderWindow& window, const sf::Vector2u& size)
		: lif::ui::Screen(window, size)
	{}

	void rebuild(ScreenBuilder&) override final {
		clear();
		build();
	}
};
}
}
