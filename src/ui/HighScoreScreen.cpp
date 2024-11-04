#include "HighScoreScreen.hpp"
#include "HighScoreManager.hpp"
#include "game.hpp"
#include "ShadedText.hpp"
#include "language.hpp"
#include "Drawable.hpp"
#include "Interactable.hpp"
#include "game.hpp"
#include "utils.hpp"

using lif::ui::HighScoreScreen;

HighScoreScreen::HighScoreScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::DynamicScreen(window, sz)
{
	build();
}

void HighScoreScreen::build() {
	name = SCREEN_NAME;
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 *
	 * HighScores
	 * [name1]...[score1]
	 * [name2]...[score2]
	 * --
	 * [Back]
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, size.x, size.y);
	const auto size = 24;
	const float ipadx = 25,
	            ipady = 15;

	auto text = new lif::ShadedText(font, lif::getLocalized("high_scores"), sf::Vector2f(ipadx, ipady));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	// Back
	text = new lif::ShadedText(font, lif::getLocalized("back"), sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	auto bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 3 * bounds.height));
	interactables["back"] = std::make_unique<lif::ui::Interactable>(text);

	nonInteractableSizeExcludingHighScores = nonInteractables.size();
}

void HighScoreScreen::onLoad() {
	Screen::onLoad();

	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, size.x, size.y);
	const auto size = 14;

	sf::Vector2f pos(25, 75);

	nonInteractables.resize(nonInteractableSizeExcludingHighScores);

	const sf::Color colors[2] = { sf::Color(255, 255, 255), sf::Color(180, 180, 180) };
	auto colorIdx = 0;

	const auto& highscores = lif::getHighScoreManager().getSortedHighScores();
	for (const auto& entry : highscores) {
		auto text = new lif::ShadedText(font, entry.name, pos);
		text->setFGColor(colors[colorIdx]);
		text->setShadowSpacing(1, 1);
		text->setCharacterSize(size);
		nonInteractables.emplace_back(text);

		text = new lif::ShadedText(font, std::to_string(entry.score), pos);
		text->setFGColor(colors[colorIdx]);
		text->setPosition(sf::Vector2f(win_bounds.width - 250, text->getPosition().y));
		text->setShadowSpacing(1, 1);
		text->setCharacterSize(size);
		nonInteractables.emplace_back(text);

		colorIdx = (colorIdx + 1) % 2;

		pos.y += 20;
	}

	if (highscores.size() == 0 && transitions.size() == 0) {
		transitions.add("back", std::make_pair(lif::Direction::DOWN, "back"));
	}
}
