#include "WinLoseHandler.hpp"
#include "MusicManager.hpp"
#include "LevelManager.hpp"
#include "GameCache.hpp"
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

State WinLoseHandler::handleWinLose() {
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

	return state;
}

void WinLoseHandler::_handleWin() {
	const auto time = clock.getElapsedTime();
	if (time >= sf::seconds(4)) {
		std::cerr << "phase3\n";
		levelClearSoundPlayed = false;
		playerWinSoundPlayed = false;
		for (unsigned short id = 1; id <= Game::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player != nullptr)
				player->setWinning(false);
		}
		state = State::ADVANCING_LEVEL;

	} else if (time >= sf::seconds(1.8) && !playerWinSoundPlayed) {
		std::cerr << "phase2\n";
		for (unsigned short id = 1; id <= Game::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player != nullptr) {
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

	_displayGetReady(window, panel, lvnum);
}

void WinLoseHandler::_displayGetReady(sf::RenderWindow& window, const Game::SidePanel& panel, short lvnum) {
	std::stringstream ss;
	ss << "LEVEL " << lvnum;
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

bool WinLoseHandler::displayContinue(sf::RenderWindow& target, const Game::SidePanel& panel, short playerId) {
	return true;
}
