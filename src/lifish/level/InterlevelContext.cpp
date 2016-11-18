#include "InterlevelContext.hpp"
#include "contexts.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "GameCache.hpp"
#include "SidePanel.hpp"
#include <iostream>

using Game::InterlevelContext;

static const sf::FloatRect WIN_BOUNDS(Game::MAIN_WINDOW_SHIFT, 0,
			Game::LEVEL_WIDTH * Game::TILE_SIZE,
			Game::LEVEL_HEIGHT * Game::TILE_SIZE);

InterlevelContext::InterlevelContext(Game::LevelManager& lm, const Game::SidePanel& sidePanel)
	: lm(lm)
	, sidePanel(sidePanel)
{
	const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
	if (!interlevelFont.loadFromFile(fontname)) {
		std::cerr << "[WinLoseHandler.cpp] Error: couldn't load font " << fontname << std::endl;
	}
	
	centralText.setFont(interlevelFont);
	centralText.setCharacterSize(13);
	subtitleText.setFont(interlevelFont);
	subtitleText.setCharacterSize(13);
}

void InterlevelContext::setAdvancingLevel() {
	state = State::DISTRIBUTING_POINTS;
	lastTickTime = clock.restart();
	bonusPoints = 0;
	bonusTime = sf::Time::Zero;
	centralText.setString("TIME BONUS!");
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(Game::center(bounds, WIN_BOUNDS));
	subtitleText.setString("0");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
}

void InterlevelContext::_setGettingReady() {
	state = State::GETTING_READY;
	clock.restart();
	centralText.setString("LEVEL " + Game::to_string(lm.getLevel()->getInfo().levelnum));
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(Game::center(bounds, WIN_BOUNDS));
	subtitleText.setString("GET READY!");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
}

void InterlevelContext::update() {
	switch (state) {
	case State::DISTRIBUTING_POINTS:
		_tickDistributePoints();
		break;
	case State::GETTING_READY:
		_tickGettingReady();
		break;
	}
}

void InterlevelContext::_tickDistributePoints() {
	const auto time = clock.getElapsedTime();
	
	// Wait 2 seconds before starting updating score
	if (time < sf::seconds(2)) return;

	// Wait 60ms before next update
	if (bonusPoints > 0 && time - lastTickTime < sf::milliseconds(60)) return;
	
	if (bonusTime == sf::Time::Zero) {
		if (bonusPoints == 0) {
			// First assignment of bonusTime
			bonusTime = sf::seconds(int(lm.getLevelTime().getRemainingTime().asSeconds()));
		} else {
			// Pass to next phase
			// TODO: prompt continue
			lm.advanceLevel();
			_setGettingReady();
			return;
		}
	}

	auto& levelTime = const_cast<Game::LevelTime&>(lm.getLevelTime());
	const auto time_bonus_sound = Game::getAsset("sounds", Game::TIME_BONUS_SOUND);
	if (bonusTime > sf::seconds(60)) {
		bonusTime -= sf::seconds(60);
		levelTime.setTime(bonusTime);
		bonusPoints += 1000;
		_givePoints(1000);
	} else {
		bonusTime -= sf::seconds(1);
		levelTime.setTime(bonusTime);
		bonusPoints += 100;
		_givePoints(100);
	}
	subtitleText.setString(Game::to_string(bonusPoints));
	const auto bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
	Game::cache.playSound(time_bonus_sound);
	lastTickTime = time;
}

void InterlevelContext::_tickGettingReady() {
	if (clock.getElapsedTime() > sf::seconds(3))
		newContext = Game::CTX_GAME;
}

bool InterlevelContext::handleEvent(sf::Window& window, sf::Event evt) {
	return false;
}
	
void InterlevelContext::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(centralText, states);
	window.draw(subtitleText, states);
	window.draw(sidePanel, states);
}

void InterlevelContext::_givePoints(int amount) {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player != nullptr)
			Game::score[i] += amount;
	}
}

/*
void WinLoseHandler::advanceLevel(sf::RenderWindow& window, const Game::SidePanel& panel) {
	// Display the time bonus on screen
	auto time_bonus = lm.getLevelTime().getRemainingTime();
	
	sf::Text time_bonus_text("TIME BONUS!", interlevelFont, 13);
	if (time_bonus > sf::Time::Zero) {
		auto bounds = time_bonus_text.getGlobalBounds();
		time_bonus_text.setPosition(Game::center(bounds, WIN_BOUNDS));

		window.clear();
		window.draw(time_bonus_text);
		
		sf::Text points_text("0", interlevelFont, 13);
		bounds = points_text.getGlobalBounds();
		points_text.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));

		window.draw(points_text);
		window.draw(panel);
		window.display();

		sf::sleep(sf::seconds(2));

		// Assign time bonus points
		unsigned int points = 0;
		auto givePoints = [this] (int amount) {
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
				auto player = lm.getPlayer(i + 1);
				if (player != nullptr)
					Game::score[i] += amount;
			}
		};
		auto& levelTime = const_cast<Game::LevelTime&>(lm.getLevelTime());
		const auto time_bonus_sound = Game::getAsset("sounds", Game::TIME_BONUS_SOUND);
		while (time_bonus > sf::Time::Zero) {
			if (time_bonus > sf::seconds(60)) {
				time_bonus -= sf::seconds(60);
				levelTime.setTime(time_bonus);
				points += 1000;
				givePoints(1000);
			} else {
				time_bonus -= sf::seconds(1);
				levelTime.setTime(time_bonus);
				points += 100;
				givePoints(100);
			}
			points_text.setString(Game::to_string(points));
			points_text.setPosition(Game::center(points_text.getGlobalBounds(), WIN_BOUNDS)
					+ sf::Vector2f(0.f, 2 * bounds.height));
			Game::cache.playSound(time_bonus_sound);

			window.clear();
			window.draw(time_bonus_text);
			window.draw(points_text);
			window.draw(panel);
			window.display();
			sf::sleep(sf::milliseconds(60));
		}

		sf::sleep(sf::seconds(2));
	}

	const auto level = lm.getLevel();
	const auto& levelSet = level->getLevelSet();
	short lvnum = level->getInfo().levelnum;

	if (lvnum == levelSet.getLevelsNum()) {
		state = State::GAME_WON;
	}

	// Resurrect any dead player which has a 'continue' left and
	// remove shield and speedy effects
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if ((player == nullptr || player->get<Game::Killable>()->isKilled())
				&& Game::playerContinues[i] > 0) 
		{
			if (_displayContinue(window, panel, i + 1)) {
				--Game::playerContinues[i];
				auto player = std::make_shared<Player>(sf::Vector2f(0, 0), i + 1);
				player->get<Game::Controllable>()->setWindow(window);
				lm.setPlayer(i + 1, player);
			} else {
				Game::playerContinues[i] = 0;
				lm.removePlayer(i + 1);
			}
		} else if (player != nullptr) {
			auto bns = player->get<Game::Bonusable>();
			bns->giveBonus(Game::BonusType::SPEEDY, sf::Time::Zero);
			bns->giveBonus(Game::BonusType::SHIELD, sf::Time::Zero);
		}
	}

	_displayGetReady(window, panel, lvnum);
}

void WinLoseHandler::_displayGetReady(sf::RenderWindow& window, const Game::SidePanel& panel, short lvnum) {
	std::stringstream ss;
	ss << "LEVEL " << (lvnum + 1);
	sf::Text text(ss.str(), interlevelFont, 13);
	text.setPosition(Game::center(text.getGlobalBounds(), WIN_BOUNDS));

	window.clear();
	window.draw(text);

	text.setString("GET READY!");
	const auto bounds = text.getGlobalBounds();
	text.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));

	window.draw(text);
	window.draw(panel);
	window.display();

	sf::sleep(sf::seconds(3));
}

bool WinLoseHandler::_displayContinue(sf::RenderWindow& window, const Game::SidePanel& panel, short playernum) {
	std::vector<sf::Text> texts;

	std::stringstream ss;
	ss << "PLAYER " << playernum << " CONTINUE?";
	sf::Text text(ss.str(), interlevelFont, 13);
	text.setPosition(Game::center(text.getGlobalBounds(), WIN_BOUNDS));
	texts.push_back(text);

	ss.str("");
	ss << "(" << Game::playerContinues[playernum-1] << " remaining)";
	text.setString(ss.str());
	auto bounds = text.getGlobalBounds();
	text.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
	texts.push_back(text);

	// Dummy text to get the correct bounds
	text.setString("YES / NO");
	text.setCharacterSize(15);
	bounds = text.getGlobalBounds();
	
	sf::Text yes_text("YES", interlevelFont, 15);
	yes_text.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 4 * bounds.height));
	yes_text.setFillColor(sf::Color::Red);

	bounds = yes_text.getGlobalBounds();
	text.setString(" / ");
	text.setPosition(sf::Vector2f(bounds.left + bounds.width, bounds.top));
	texts.push_back(text);

	bounds = text.getGlobalBounds();
	sf::Text no_text("NO", interlevelFont, 15);
	no_text.setPosition(sf::Vector2f(bounds.left + bounds.width, bounds.top));

	text.setString("Arrows / Enter to select");
	text.setCharacterSize(10);
	text.setPosition(Game::center(text.getGlobalBounds(), WIN_BOUNDS) + sf::Vector2f(0.f, 6 * bounds.height));
	texts.push_back(text);

	auto drawTexts = [&texts, &window] {
		for (const auto& text : texts)
			window.draw(text);
	};

	drawTexts();
	window.draw(panel);
	window.display();

	bool yes_selected = true;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				return false;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Left:
					yes_selected = true;
					break;
				case sf::Keyboard::Right:
					yes_selected = false;
					break;
				case sf::Keyboard::Return:
					return yes_selected;
				case sf::Keyboard::Key::F:
					Game::options.showFPS = !Game::options.showFPS;
					break;
				default:
					break;
				}
			default:
				break;
			}
		}

		if (yes_selected) {
			yes_text.setFillColor(sf::Color::Red);
			no_text.setFillColor(sf::Color::White);
		} else {
			yes_text.setFillColor(sf::Color::White);
			no_text.setFillColor(sf::Color::Red);
		}

		window.clear();
		window.draw(panel);
		drawTexts();
		window.draw(yes_text);
		window.draw(no_text);
		Game::maybeShowFPS(window);
		window.display();	
	}

	return false;
}*/
