#include "WinLoseHandler.hpp"
#include "LevelManager.hpp"

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
	
}

void WinLoseHandler::_handleLoss() {
	if (lm.dropTextManager.isPlaying(Game::DroppingTextManager::Text::GAME_OVER)) 
		return;

	// TODO: back to home screen
}

void WinLoseHandler::_checkCondition() {
	if (lm.isGameOver()) {
		state = State::HANDLING_LOSS;
		lm.dropTextManager.trigger(Game::DroppingTextManager::Text::GAME_OVER);
		_handleLoss();
	} else if (lm.isLevelClear()) {
		state = State::HANDLING_WIN;
		_handleWin();
	}
}
