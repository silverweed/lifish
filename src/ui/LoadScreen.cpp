#include "LoadScreen.hpp"
#include "ShadedText.hpp"
#include "language.hpp"
#include "Drawable.hpp"
#include "Interactable.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "SaveScreen.hpp"
#include <memory>
#include <SFML/Graphics.hpp>

using lif::ui::LoadScreen;

namespace lif {
	namespace ui {
		extern std::string screenCallbackArg;
	}
}

LoadScreen::LoadScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::DynamicScreen(window, sz)
{
	build();
}

void LoadScreen::build() {
	name = SCREEN_NAME;
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 *
	 * Load
	 * [x][save1]
	 * [x][save2]
	 * --
	 * [Back]
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, size.x, size.y);
	const auto size = 24;
	const float ipadx = 25,
	            ipady = 15;

	auto text = new lif::ShadedText(font, lif::getLocalized("load_game"), sf::Vector2f(ipadx, ipady));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	// Back
	text = new lif::ShadedText(font, lif::getLocalized("back"), sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	auto bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 3 * bounds.height));
	interactables["back"] = std::make_unique<lif::ui::Interactable>(text);
}

void LoadScreen::onLoad() {
	Screen::onLoad();

	selected = std::make_pair("", nullptr);

	// Wipe out old elements
	{
		auto begin = nonInteractables.begin();
		if (begin != nonInteractables.end()) {
			++begin;
			nonInteractables.erase(begin, nonInteractables.end());
		}
	}
	for (auto it = interactables.begin(); it != interactables.end(); ) {
		if (it->first != "back")
			it = interactables.erase(it);
		else
			++it;
	}
	callbacks.clear();
	transitions.clear();

	const auto size = 20;
	const auto font = lif::getAsset("fonts", lif::fonts::CUTSCENES);

	sf::Vector2f pos(25, 75);

	auto saves = browseSaveData(lif::getSaveDir());
	if (saves.size() == 0) {
		auto text = new lif::ShadedText(font, lif::getLocalized("no_save_data"), pos);
		text->setCharacterSize(size);
		text->setShadowSpacing(2, 2);
		text->setStyle(sf::Text::Italic);
		nonInteractables.emplace_back(text);
		// Add a dummy transition to allow selecting this with keyboard/joystick
		transitions.add("back", std::make_pair(lif::Direction::DOWN, "back"));
		return;
	}

	unsigned col = 0,
	         row = 0;
	// These variables store the previous load callback and delete callback
	std::string prevLcbname, prevDcbname;
	for (const auto& save : saves) {
		// Setup delete button
		auto text = new lif::ShadedText(font, "[x]", pos);
		text->setCharacterSize(size);
		const auto dcbname = _newUniqueId();
		interactables[dcbname] = std::make_unique<lif::ui::Interactable>(text);
		callbacks[dcbname] = [this, save] () {
			_createDeleteCallback(save.path)();
			// Reload load screen to refresh list of save files
			lif::ui::screenCallbackArg = "load";
			return lif::ui::Action::SWITCH_SCREEN;
		};

		// Setup load button
		text = new lif::ShadedText(font,
			save.displayName.substr(0, lif::ui::SaveScreen::MAX_SAVE_FILE_NAME_LEN)
				+ " (lv " + lif::to_string(save.level) + ", " + lif::to_string(save.nPlayers) + "P)",
			pos + sf::Vector2f(35, 0));
		text->setCharacterSize(size);
		const auto lcbname = _newUniqueId();
		interactables[lcbname] = std::make_unique<lif::ui::Interactable>(text);
		callbacks[lcbname] = _createLoadCallback(save.path);

		transitions.add(lcbname, std::make_pair(lif::Direction::LEFT, dcbname));
		if (prevLcbname.length() > 0) {
			transitions.add(lcbname, std::make_pair(lif::Direction::UP, prevLcbname));
			transitions.add(dcbname, std::make_pair(lif::Direction::UP, prevDcbname));
		}
		prevLcbname = lcbname;
		prevDcbname = dcbname;

		// Calculate new position
		if (++col == 10) {
			col = 0;
			pos.x += 300;
			pos.y = 75;
			if (++row == 2)
				break;
		} else {
			pos.y += 30;
		}
	}

	transitions.add("back", std::make_pair(lif::Direction::UP, prevLcbname));
}
