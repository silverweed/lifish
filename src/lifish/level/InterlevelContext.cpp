#include "InterlevelContext.hpp"
#include "language.hpp"
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

	// Position yes / no texts
	sf::Text dummyText("YES / NO_", interlevelFont, 13);
	auto bounds = dummyText.getGlobalBounds();
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

	if (lm.getLevelTime().getRemainingTime() <= sf::Time::Zero) {
		// No time remaining: skip this phase
		_setPromptContinue();
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

void InterlevelContext::_setPromptContinue() {
	// Check if there are dead players with continues left
	mustPromptPlayer.fill(false);
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		const auto player = lm.getPlayer(i + 1);
		if ((player == nullptr || player->get<lif::Killable>()->isKilled())
				&& lm.getPlayerContinues(i + 1) > 0) {
			mustPromptPlayer[i] = true;
		}
	}
	unsigned idx = 0;
	while (idx < mustPromptPlayer.size() && !mustPromptPlayer[idx]) ++idx;
	if (idx == mustPromptPlayer.size()) {
		// all players alive or without continues: skip this phase
		_setGettingReady();
		return;
	}
	state = State::PROMPT_CONTINUE;
	time = sf::Time::Zero;
	_preparePromptContinue(idx);
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
	// Wait 2 seconds before starting updating score
	if (time < sf::seconds(2)) return;

	// Wait 60ms before next update
	if (bonusPoints > 0 && time - lastTickTime < sf::milliseconds(60)) return;

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

	if (time > sf::seconds(3))
		newContext = lif::CTX_GAME;
}

void InterlevelContext::_tickWaitDistributePoints() {
	if (time > sf::seconds(2))
		_setPromptContinue();
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

	if (curPromptedPlayer < mustPromptPlayer.size())
		_preparePromptContinue(curPromptedPlayer);
	else  {
		// all due players were prompted
		yesText.setFillColor(sf::Color(0, 0, 0, 0));
		noText.setFillColor(sf::Color(0, 0, 0, 0));
		subsubtitleText.setString("");
		_setGettingReady();
	}
}

void InterlevelContext::_setGettingReady() {
	int advance = 1;
	if (retryingLevel) {
		advance = 0;
		retryingLevel = false;
	}
	setGettingReady(lm.getLevel()->getInfo().levelnum + advance);
}

bool InterlevelContext::handleEvent(sf::Window&, sf::Event event) {
	if (state != State::PROMPT_CONTINUE) return false;
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
	window.draw(sidePanel, states);
}

void InterlevelContext::_givePoints(int amount) {
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player != nullptr)
			lm.addScore(i + 1, amount);
	}
}
