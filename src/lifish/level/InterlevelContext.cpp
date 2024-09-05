#include "InterlevelContext.hpp"
#include "language.hpp"
#include "HighScoreManager.hpp"
#include "BaseEventHandler.hpp"
#include "GameCache.hpp"
#include "JoystickManager.hpp"
#include "Killable.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "Player.hpp"
#include "SidePanel.hpp"
#include "Time.hpp"
#include "contexts.hpp"
#include "controls.hpp"
#include "input_utils.hpp"
#include <iostream>

using lif::InterlevelContext;

static const sf::FloatRect WIN_BOUNDS(lif::MAIN_WINDOW_SHIFT, 0, lif::GAME_WIDTH, lif::GAME_HEIGHT);

InterlevelContext::InterlevelContext(lif::LevelManager& lm, const lif::SidePanel& sidePanel)
	: lif::WindowContext()
	, lm(lm)
	, sidePanel(sidePanel)
{
	_addHandler<lif::BaseEventHandler>();

	const std::string fontname = lif::getAsset("fonts", lif::fonts::INTERLEVEL);
	if (!interlevelFont.loadFromFile(fontname)) {
		std::cerr << "[WinLoseHandler.cpp] Error: couldn't load font " << fontname << std::endl;
	}

	centralText.setFont(interlevelFont);
	centralText.setCharacterSize(13);
	subtitleText.setFont(interlevelFont);
	subtitleText.setCharacterSize(13);
	subsubtitleText.setFont(interlevelFont);
	subsubtitleText.setCharacterSize(13);
	yesText.setFont(interlevelFont);
	yesText.setCharacterSize(13);
	noText.setFont(interlevelFont);
	noText.setCharacterSize(13);

	// Cursor
	bufferText = &subtitleText;
	bufferText->setString("D");
	auto bounds = bufferText->getGlobalBounds();
	charBounds = sf::Vector2f(bounds.width, bounds.height);
	bufferText->setString("");
	cursor.setSize(sf::Vector2f(charBounds.x, 6));
	_updateCursorPosition();

	// Position yes / no texts
	sf::Text dummyText("YES / NO_", interlevelFont, 13);
	bounds = dummyText.getGlobalBounds();
	dummyText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(0, 2 * bounds.height));
	yesText.setString("YES");
	yesText.setPosition(dummyText.getPosition());
	dummyText.setString("YES /_");
	bounds = dummyText.getGlobalBounds();
	noText.setString("NO");
	noText.setPosition(dummyText.getPosition() + sf::Vector2f(bounds.width, 0));
	yesText.setFillColor(sf::Color(255, 0, 0, 0));
	noText.setFillColor(sf::Color(255, 255, 255, 0));
}

void InterlevelContext::setRetryingLevel() {
	retryingLevel = true;
	setAdvancingLevel();
}

void InterlevelContext::setAdvancingLevel() {
	lif::time.resume();

	const bool allPlayersDead = _calcPrompts();
	if (lm.getLevelTime().getRemainingTime() <= sf::Time::Zero || allPlayersDead) {
		// No time remaining or all players dead: skip this phase
		_setNextPrompt();
		return;
	}
	state = State::DISTRIBUTING_POINTS;
	lastTickTime = time = sf::Time::Zero;
	bonusPoints = 0;
	bonusTime = sf::Time::Zero;
	centralText.setString(lif::getLocalized("time_bonus"));
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(lif::center(bounds, WIN_BOUNDS));
	subtitleText.setString("0");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
}

void InterlevelContext::setGettingReady(unsigned short lvnum) {
	lif::time.resume();

	state = State::GETTING_READY;
	time = sf::Time::Zero;
	centralText.setString(lif::getLocalized("level") + " " + lif::to_string(lvnum));
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(lif::center(bounds, WIN_BOUNDS));
	subtitleText.setString(lif::getLocalized("get_ready"));
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
}

bool InterlevelContext::_calcPrompts() {
	const auto& highScoreMgr = lif::getHighScoreManager();
	auto allPlayersAreDead = true;

	mustPromptPlayer.fill(0);
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		const auto player = lm.getPlayer(i + 1);
		if ((player == nullptr || player->get<lif::Killable>()->isKilled())) {
			if (lm.getPlayerContinues(i + 1) > 0)
				mustPromptPlayer[i] |= PROMPT_REVIVE;

			if (highScoreMgr.isHighScore(lm.getScore(i + 1))) {
				mustPromptPlayer[i] |= PROMPT_HIGHSCORE;
			}
		} else {
			allPlayersAreDead = false;
		}
	}

	return allPlayersAreDead;
}

void InterlevelContext::_setNextPrompt() {
	time = sf::Time::Zero;

	for (unsigned idx = 0; idx < mustPromptPlayer.size(); ++idx) {
		if (mustPromptPlayer[idx] & PROMPT_HIGHSCORE) {
			mustPromptPlayer[idx] &= ~PROMPT_HIGHSCORE;
			state = State::PROMPT_HIGHSCORE;
			_preparePromptHighScore(idx);
			return;
		}

		if (mustPromptPlayer[idx] & PROMPT_REVIVE) {
			mustPromptPlayer[idx] &= ~PROMPT_REVIVE;
			state = State::PROMPT_CONTINUE;
			_preparePromptContinue(idx);
			return;
		}
	}

	_setGettingReady();
}

void InterlevelContext::_preparePromptContinue(unsigned short idx) {
	curPromptedPlayer = idx;
	centralText.setString("P" + lif::to_string(idx+1) + " CONTINUE? ("
			+ lif::to_string(lm.getPlayerContinues(idx + 1)) + " left)");
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(lif::center(bounds, WIN_BOUNDS));
	subtitleText.setString("/");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
	subsubtitleText.setString("Arrows / Enter to select");
	subsubtitleText.setCharacterSize(13);
	bounds = subsubtitleText.getGlobalBounds();
	subsubtitleText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 4 * bounds.height));
	subsubtitleText.setCharacterSize(10);
	bounds = subsubtitleText.getGlobalBounds();
	subsubtitleText.setPosition(lif::centerX(bounds, WIN_BOUNDS), bounds.top);
	yesText.setFillColor(sf::Color::Red);
	yesText.setCharacterSize(15);
	noText.setFillColor(sf::Color::White);
	noText.setCharacterSize(13);
}

void InterlevelContext::_preparePromptHighScore(unsigned short idx) {
	curPromptedPlayer = idx;
	centralText.setString("P" + lif::to_string(idx+1) + " HIGHSCORE! NAME?");
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(lif::center(bounds, WIN_BOUNDS));

	subtitleText.setString("|");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(-100.0, 2 * bounds.height));
	subtitleText.setString("_");
}

void InterlevelContext::update() {
	time += lif::time.getDelta();
	switch (state) {
	case State::DISTRIBUTING_POINTS:
		_tickDistributePoints();
		break;
	case State::GETTING_READY:
		_tickGettingReady();
		break;
	case State::WAIT_DISTRIBUTING_POINTS:
		_tickWaitDistributePoints();
		// fallthrough
	case State::PROMPT_HIGHSCORE:
		break;
	case State::PROMPT_CONTINUE:
		// Selection via joystick (can't do this is the event handler since it's not an SFML event)
		{
			const auto& jl = lif::joystick::JoystickManager::getInstance().getListener(curPromptedPlayer);
			if (jl.evtMoved(lif::joystick::JoystickListener::Axis::L_LEFT)) {
				_continueSelectYes();
			} else if (jl.evtMoved(lif::joystick::JoystickListener::Axis::L_RIGHT)) {
				_continueSelectNo();
			}
		}
	default:
		break;
	}
}

void InterlevelContext::_tickDistributePoints() {
	// Wait a bit before starting updating score
	if (time < sf::seconds(1.8)) return;

	// Wait a bit before next update
	if (bonusPoints > 0 && time - lastTickTime < sf::milliseconds(40)) return;

	if (bonusTime <= sf::Time::Zero) {
		if (bonusPoints == 0) {
			// First assignment of bonusTime
			bonusTime = sf::milliseconds(lm.getLevelTime().getRemainingTime().asMilliseconds());
		} else {
			// Pass to next phase
			time = sf::Time::Zero;
			state = State::WAIT_DISTRIBUTING_POINTS;
			return;
		}
	}

	auto& levelTime = const_cast<lif::LevelTime&>(lm.getLevelTime());
	const auto time_bonus_sound = lif::getAsset("sounds", lif::TIME_BONUS_SOUND);
	if (bonusTime > sf::seconds(60)) {
		bonusTime -= sf::seconds(60);
		levelTime.setTime(bonusTime);
		bonusPoints += 600;
		_givePoints(600);
	} else {
		bonusTime -= sf::seconds(1);
		levelTime.setTime(bonusTime);
		bonusPoints += 10;
		_givePoints(10);
	}
	subtitleText.setString(lif::to_string(bonusPoints));
	const auto bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(lif::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
	lif::cache.playSound(time_bonus_sound);
	lastTickTime = time;
}

void InterlevelContext::_tickGettingReady() {
	// Skip this phase if player clicks mouse or bomb button
	if (time > sf::milliseconds(300) &&
		(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ||
		std::any_of(std::begin(lif::controls::players), std::end(lif::controls::players), [] (const auto& keys) {
			return IS_KEY_PRESSED(keys[lif::controls::CTRL_BOMB]);
		}))
	) {
		time = sf::seconds(4);
	}

	if (time > sf::seconds(3)) {
		if (anyNewHighScores)
			lif::getHighScoreManager().saveHighScores();
		newContext = lif::CTX_GAME;
	}
}

void InterlevelContext::_tickWaitDistributePoints() {
	if (time > sf::seconds(2))
		_setNextPrompt();
}

void InterlevelContext::_ackPromptResponse() {
	// If player wants to continue, don't do anything here: it'll be the GameContext
	// to check if the player has spare continues, and will resurrect it if so.
	// Otherwise, zero its continues so it won't be resurrected.
	if (!yesSelected)
		lm.setPlayerContinues(curPromptedPlayer + 1, 0);
	do {
		++curPromptedPlayer;
	} while (!mustPromptPlayer[curPromptedPlayer] && curPromptedPlayer < mustPromptPlayer.size());

	yesText.setFillColor(sf::Color(0, 0, 0, 0));
	noText.setFillColor(sf::Color(0, 0, 0, 0));
	subsubtitleText.setString("");
	_setNextPrompt();
}

void InterlevelContext::_setGettingReady() {
	int advance = 1;
	if (retryingLevel) {
		advance = 0;
		retryingLevel = false;
	}
	setGettingReady(lm.getLevel()->getInfo().levelnum + advance);
}

bool InterlevelContext::_handleEventPromptHighscore(sf::Event event) {
	if (event.type != sf::Event::KeyPressed)
		return false;

	switch (event.key.code) {
	case sf::Keyboard::BackSpace:
		// Cancel
		if (IS_KEY_PRESSED(sf::Keyboard::LControl)) {
			buffer.fill('\0');
			bufIdx = 0;
		} else if (bufIdx > 0) {
			buffer[--bufIdx] = '\0';
		}
		bufferText->setString(std::string(buffer.data(), bufIdx));
		_updateCursorPosition();
		return true;
	case sf::Keyboard::Return:
		// Confirm
		if (bufIdx > 0) {
			const auto entry = lif::HighScoreEntry {
				std::string(buffer.data(), bufIdx),
				lm.getScore(curPromptedPlayer + 1)
			};
			lif::getHighScoreManager().addHighScore(entry);
			anyNewHighScores = true;
			cursorVisible = false;
			buffer.fill('\0');
			bufIdx = 0;
			_setNextPrompt();
			return true;
		} else {
			return false;
		}
	default:
		break;
	}

	const char ch = lif::kb::keyToAlnum(event.key.code);
	if (ch == (char)-1)
		return false;

	if (bufIdx < buffer.size()) {
		buffer[bufIdx++] = ch;
		bufferText->setString(std::string(buffer.data(), bufIdx));
		_updateCursorPosition();
	}

	return true;
}

bool InterlevelContext::_handleEventPromptContinue(sf::Event event) {
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton.button;
			if (lif::joystick::isButton(lif::joystick::ButtonType::BTN_DOWN, curPromptedPlayer, btn)
				|| lif::joystick::isButton(lif::joystick::ButtonType::START, curPromptedPlayer, btn))
			{
				_ackPromptResponse();
				return true;
			}
		}
		break;
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Left:
			_continueSelectYes();
			return true;
		case sf::Keyboard::Right:
			_continueSelectNo();
			return true;
		case sf::Keyboard::Return:
			_ackPromptResponse();
			return true;
		default:
			break;
		}
	default:
		break;
	}

	return false;
}

bool InterlevelContext::handleEvent(sf::Window&, sf::Event event) {
	if (state == State::PROMPT_CONTINUE)
		return _handleEventPromptContinue(event);

	else if (state == State::PROMPT_HIGHSCORE)
		return _handleEventPromptHighscore(event);

	return false;
}

void InterlevelContext::_updateCursorPosition() {
	const auto txtbounds = bufferText->getGlobalBounds();
	cursor.setPosition(bufferText->getPosition() + sf::Vector2f(txtbounds.width, charBounds.y));
	cursorVisible = true;
	cursor.setFillColor(sf::Color::White);
	cursorClock.restart();
}

void InterlevelContext::_continueSelectYes() {
	yesSelected = true;
	yesText.setFillColor(sf::Color::Red);
	yesText.setCharacterSize(15);
	noText.setFillColor(sf::Color::White);
	noText.setCharacterSize(13);
}

void InterlevelContext::_continueSelectNo() {
	yesSelected = false;
	noText.setFillColor(sf::Color::Red);
	noText.setCharacterSize(15);
	yesText.setFillColor(sf::Color::White);
	yesText.setCharacterSize(13);
}

void InterlevelContext::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(centralText, states);
	window.draw(subtitleText, states);
	window.draw(yesText, states);
	window.draw(noText, states);
	window.draw(subsubtitleText, states);
	if (cursorVisible) window.draw(cursor, states);
	window.draw(sidePanel, states);
}

void InterlevelContext::_givePoints(int amount) {
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player != nullptr)
			lm.addScore(i + 1, amount);
	}
}
