#include "WinLoseHandler.hpp"
#include "MusicManager.hpp"
#include "LevelManager.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Bonusable.hpp"
#include "Controllable.hpp"
#include "Music.hpp"
#include "Options.hpp"
#include "game.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "Player.hpp"
#include "Sounded.hpp"
#include "SidePanel.hpp"
#include <iostream>

using Game::WinLoseHandler;
using State = WinLoseHandler::State;

WinLoseHandler::WinLoseHandler(Game::LevelManager& lm)
	: lm(lm)
{
	const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
	if (!interlevelFont.loadFromFile(fontname)) {
		std::cerr << "[WinLoseHandler.cpp] Error: couldn't load font " << fontname << std::endl;
	}
}

void WinLoseHandler::handleWinLose() {
	switch (state) {
	case State::HANDLING_WIN:
		_handleWin();
		break;
	case State::HANDLING_LOSS:
		_handleLoss();
		break;
	case State::ADVANCING_LEVEL:
		state = State::DEFAULT;
		break;
	default:
		_checkCondition();
		break;
	}
}

void WinLoseHandler::_handleWin() {
	const auto time = clock.getElapsedTime();
	if (time >= sf::seconds(4)) {
		std::cerr << "phase3\n";
		levelClearSoundPlayed = false;
		playerWinSoundPlayed = false;
		state = State::ADVANCING_LEVEL;

	} else if (time >= sf::seconds(1.8) && !playerWinSoundPlayed) {
		std::cerr << "phase2\n";
		for (unsigned short id = 1; id <= Game::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player != nullptr && !player->get<Game::Killable>()->isKillInProgress()) {
				Game::cache.playSound(player->get<Game::Sounded>()->getSoundFile(Game::Sounds::WIN));
				player->setWinning(true);
			}
		}
		playerWinSoundPlayed = true;

	} else if (time >= sf::seconds(1) && !levelClearSoundPlayed) {
		std::cerr << "phase1\n";
		Game::musicManager->stop();
		Game::cache.playSound(Game::getAsset("test", Game::LEVEL_CLEAR_SOUND));
		levelClearSoundPlayed = true;
	}
}

void WinLoseHandler::_handleLoss() {
	if (lm.dropTextManager.isPlaying(Game::DroppingTextManager::Text::GAME_OVER))
		return;

	// TODO: back to home screen
	Game::terminated = true;
}

void WinLoseHandler::_checkCondition() {
	if (lm.isGameOver()) {
		state = State::HANDLING_LOSS;
		lm.dropTextManager.trigger(Game::DroppingTextManager::Text::GAME_OVER);
		_handleLoss();
	} else if (lm.isLevelClear()) {
		state = State::HANDLING_WIN;
		clock.restart();
		_handleWin();
	}
}

void WinLoseHandler::advanceLevel(sf::RenderWindow& window, const Game::SidePanel& panel) {
	// Display the time bonus on screen
	auto time_bonus = lm.getLevelTime().getRemainingTime();
	
	sf::Text time_bonus_text("TIME BONUS!", interlevelFont, 13);
	if (time_bonus > sf::Time::Zero) {
		auto bounds = time_bonus_text.getGlobalBounds();
		time_bonus_text.setPosition(Game::center(bounds));

		window.clear();
		window.draw(time_bonus_text);
		
		sf::Text points_text("0", interlevelFont, 13);
		bounds = points_text.getGlobalBounds();
		points_text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));

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
			points_text.setPosition(Game::center(points_text.getGlobalBounds()) + sf::Vector2f(0.f, 2 * bounds.height));
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
	text.setPosition(Game::center(text.getGlobalBounds()));

	window.clear();
	window.draw(text);

	text.setString("GET READY!");
	const auto bounds = text.getGlobalBounds();
	text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));

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
	text.setPosition(Game::center(text.getGlobalBounds()));
	texts.push_back(text);

	ss.str("");
	ss << "(" << Game::playerContinues[playernum-1] << " remaining)";
	text.setString(ss.str());
	auto bounds = text.getGlobalBounds();
	text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));
	texts.push_back(text);

	// Dummy text to get the correct bounds
	text.setString("YES / NO");
	text.setCharacterSize(15);
	bounds = text.getGlobalBounds();
	
	sf::Text yes_text("YES", interlevelFont, 15);
	yes_text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 4 * bounds.height));
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
	text.setPosition(Game::center(text.getGlobalBounds()) + sf::Vector2f(0.f, 6 * bounds.height));
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
}
