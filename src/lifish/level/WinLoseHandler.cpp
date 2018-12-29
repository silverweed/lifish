#include "WinLoseHandler.hpp"
#include "Bonusable.hpp"
#include "Controllable.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "Music.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "SidePanel.hpp"
#include "Sounded.hpp"
#include "Time.hpp"
#include "game.hpp"
#include <iostream>

using lif::WinLoseHandler;
using State = WinLoseHandler::State;

WinLoseHandler::WinLoseHandler(lif::LevelManager& lm, const lif::SidePanel& sidePanel)
	: lm(lm)
	, interlevelCtx(lm, sidePanel)
{}

void WinLoseHandler::handleWinLose() {
	time += lif::time.getDelta();
	switch (state) {
	case State::HANDLING_WIN:
		_handleWin();
		break;
	case State::HANDLING_LOSS:
		_handleLoss();
		break;
	case State::ADVANCING_LEVEL:
	case State::RETRY_LEVEL:
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
	if (time >= sf::seconds(4)) {
		levelClearSoundPlayed = false;
		playerWinSoundPlayed = false;
		// Ensure all players have no pending resurrect
		for (unsigned id = 1; id <= lif::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player == nullptr) continue;
			auto klb = player->get<lif::Killable>();
			if (klb->isKilled())
				lm._tryResurrectPlayer(player);
		}
		lm.pause();
		interlevelCtx.setAdvancingLevel();
		state = State::ADVANCING_LEVEL;

	} else if (time >= sf::seconds(1.8) && !playerWinSoundPlayed) {
		for (unsigned id = 1; id <= lif::MAX_PLAYERS; ++id) {
			auto player = lm.getPlayer(id);
			if (player != nullptr && !player->get<lif::Killable>()->isKillInProgress()) {
				lif::cache.playSound(player->get<lif::Sounded>()->getSoundFile("win"));
				player->setWinning(true);
			}
		}
		playerWinSoundPlayed = true;

	} else if (time >= sf::seconds(1) && !levelClearSoundPlayed) {
		lif::musicManager->stop();
		lif::cache.playSound(lif::getAsset("sounds", lif::LEVEL_CLEAR_SOUND));
		levelClearSoundPlayed = true;
	}
}

void WinLoseHandler::_handleLoss() {
	// Wait for the GAMEOVER text to stop playing
	if (lm.dropTextManager.isPlaying(lif::DroppingTextManager::Text::GAME_OVER))
		return;

	state = State::EXIT_GAME;
}

void WinLoseHandler::_checkCondition() {
	if (lm.mustRetryLevel()) {
		state = State::RETRY_LEVEL;
		lm.levelTime->setTime(sf::Time::Zero);
		interlevelCtx.setRetryingLevel();
	} else if (lm.isGameOver()) {
		state = State::HANDLING_LOSS;
		lm.dropTextManager.trigger(lif::DroppingTextManager::Text::GAME_OVER);
		lif::musicManager->stop();
		lif::cache.playSound(lif::getAsset("sounds", lif::GAME_OVER_SOUND));
		_handleLoss();
	} else if (lm.isLevelClear()) {
		state = State::HANDLING_WIN;
		time = sf::Time::Zero;
		_handleWin();
	}
}
