#include "WinLoseHandler.hpp"
#include "MusicManager.hpp"
#include "LevelManager.hpp"
#include "GameCache.hpp"
#include "game.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "Player.hpp"
#include "Sounded.hpp"
#include <iostream>

using Game::WinLoseHandler;

WinLoseHandler::WinLoseHandler(Game::LevelManager& lm)
	: lm(lm)
{}

void WinLoseHandler::handleWinLose() {
	switch (state) {
	case State::HANDLING_WIN:
		_handleWin();
		break;
	case State::HANDLING_LOSS:
		_handleLoss();
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
		//auto& level = advance_level(window, lm, panel);
		//Game::musicManager->set(level->get<Game::Music>()->getMusic())
			//.setVolume(Game::options.musicVolume)
			//.play();
		auto level = lm.getLevel()->getLevelSet().getLevel(lm.getLevel()->getInfo().levelnum + 1);
		lm.setLevel(*level);
		levelClearSoundPlayed = false;
		playerWinSoundPlayed = false;
		for (unsigned short id = 1; id <= Game::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player != nullptr)
				player->setWinning(false);
		}
		state = State::DEFAULT;

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

/*
std::unique_ptr<Game::Level> WinLoseHandler::advanceLevel(sf::RenderWindow& window, Game::SidePanel& panel) {
	// Display the time bonus on screen
	auto time_bonus = lr.getTimeLeft();
	
	sf::Text time_bonus_text("TIME BONUS!", interlevel_font, 13);
	if (time_bonus > 0) {
		auto bounds = time_bonus_text.getGlobalBounds();
		time_bonus_text.setPosition(Game::center(bounds));

		window.clear();
		window.draw(time_bonus_text);
		
		sf::Text points_text("0", interlevel_font, 13);
		bounds = points_text.getGlobalBounds();
		points_text.setPosition(Game::center(bounds) + sf::Vector2f(0.f, 2 * bounds.height));

		window.draw(points_text);
		panel.draw(window);
		window.display();

		SLEEP_MS(2000);

		// Assign time bonus points
		unsigned int points = 0;
		auto givePoints = [&lr] (int amount) {
			auto players = lr.getPlayers();
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
				if (players[i] != nullptr)
					Game::score[i] += amount;
		};
		auto level = lr.getLevel();
		const auto time_bonus_sound = Game::getAsset("sounds", Game::TIME_BONUS_SOUND);
		while (time_bonus > 0) {
			if (time_bonus > 60) {
				time_bonus -= 60;
				level->setTime(time_bonus);
				points += 1000;
				givePoints(1000);
			} else {
				--time_bonus;
				level->setTime(time_bonus);
				points += 100;
				givePoints(100);
			}
			points_text.setString(Game::to_string(points));
			points_text.setPosition(Game::center(points_text.getGlobalBounds()) + sf::Vector2f(0.f, 2 * bounds.height));
			Game::cache.playSound(time_bonus_sound);

			window.clear();
			window.draw(time_bonus_text);
			window.draw(points_text);
			lr.resetClocks();
			panel.draw(window);
			window.display();
			SLEEP_MS(60);
		}

		SLEEP_MS(2000);
	}

	const auto level = lr.getLevel();
	const auto levelSet = level->getLevelSet();
	short lvnum = level->getLevelNum();

	if (lvnum == levelSet->getLevelsNum()) {
		// TODO: WIN!
		return nullptr;
	} else {
		++lvnum;
	}

	// Resurrect any dead player which has a 'continue' left and
	// remove shield and speedy effects
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto player = lr.getPlayer(i+1);
		if (player == nullptr && Game::playerContinues[i] > 0) {
			if (display_continue(window, panel, i+1)) {
				--Game::playerContinues[i];
				lr.setPlayer(i+1, new Game::Player(sf::Vector2f(0, 0), i+1));
			} else {
				Game::playerContinues[i] = 0;
			}
		} else if (player != nullptr) {
			player->giveShield(0);
			player->giveSpeedy(0);
		}
	}

	display_get_ready(window, panel, lvnum);

	delete level;
	lr.loadLevel(levelSet->getLevel(lvnum));

	return const_cast<Game::Level*>(lr.getLevel());
}
*/
