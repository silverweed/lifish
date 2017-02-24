#include "LoadScreen.hpp"
#include "ShadedText.hpp"
#include "Drawable.hpp"
#include "Interactable.hpp"
#include "game.hpp"
#include <memory>
#include <SFML/Graphics.hpp>

using lif::ui::LoadScreen;

LoadScreen::LoadScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::Screen(window, sz)
{
	name = "load";
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 *
	 * Load
	 * [save1]
	 * [save2]
	 * --
	 * 
	 * [save1]
	 * [new save]
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, sz.x, sz.y);
	const auto size = 24;
	const float ipadx = 25,
		    ipady = 15;

	auto text = new lif::ShadedText(font, "LOAD GAME", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));
}
