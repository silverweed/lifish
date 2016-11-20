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

static const sf::FloatRect WIN_BOUNDS(Game::MAIN_WINDOW_SHIFT, 0,
			Game::LEVEL_WIDTH * Game::TILE_SIZE,
			Game::LEVEL_HEIGHT * Game::TILE_SIZE);

WinLoseHandler::WinLoseHandler(Game::LevelManager& lm, const Game::SidePanel& sidePanel)
	: lm(lm)
	, interlevelCtx(lm, sidePanel)
{}

void WinLoseHandler::handleWinLose() {
	switch (state) {
	case State::HANDLING_WIN:
		_handleWin();
		break;
	case State::HANDLING_LOSS:
		_handleLoss();
		break;
	case State::ADVANCING_LEVEL:
		state = State::ADVANCED_LEVEL;
		break;
	case State::ADVANCED_LEVEL:
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
		levelClearSoundPlayed = false;
		playerWinSoundPlayed = false;
		lm.pause();
		interlevelCtx.setAdvancingLevel();
		state = State::ADVANCING_LEVEL;

	} else if (time >= sf::seconds(1.8) && !playerWinSoundPlayed) {
		for (unsigned short id = 1; id <= Game::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player != nullptr && !player->get<Game::Killable>()->isKillInProgress()) {
				Game::cache.playSound(player->get<Game::Sounded>()->getSoundFile(Game::Sounds::WIN));
				player->setWinning(true);
			}
		}
		playerWinSoundPlayed = true;

	} else if (time >= sf::seconds(1) && !levelClearSoundPlayed) {
		Game::musicManager->stop();
		Game::cache.playSound(Game::getAsset("test", Game::LEVEL_CLEAR_SOUND));
		levelClearSoundPlayed = true;
	}
}

void WinLoseHandler::_handleLoss() {
	// Wait for the GAMEOVER text to stop playing
	if (lm.dropTextManager.isPlaying(Game::DroppingTextManager::Text::GAME_OVER))
		return;

	state = State::EXIT_GAME;
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
