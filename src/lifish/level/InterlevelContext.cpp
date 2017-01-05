#include "InterlevelContext.hpp"
#include "contexts.hpp"
#include "Level.hpp"
#include "BaseEventHandler.hpp"
#include "LevelManager.hpp"
#include "Killable.hpp"
#include "Player.hpp"
#include "GameCache.hpp"
#include "SidePanel.hpp"
#include <iostream>

using Game::InterlevelContext;

static const sf::FloatRect WIN_BOUNDS(Game::MAIN_WINDOW_SHIFT, 0,
			Game::LEVEL_WIDTH * Game::TILE_SIZE,
			Game::LEVEL_HEIGHT * Game::TILE_SIZE);

InterlevelContext::InterlevelContext(Game::LevelManager& lm, const Game::SidePanel& sidePanel)
	: Game::WindowContext()
	, lm(lm)
	, sidePanel(sidePanel)
{
	handlers.push_back(std::unique_ptr<Game::EventHandler>(new Game::BaseEventHandler));

	const std::string fontname = Game::getAsset("fonts", Game::Fonts::INTERLEVEL);
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
	dummyText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0, 2 * bounds.height));
	yesText.setString("YES");
	yesText.setPosition(dummyText.getPosition());
	dummyText.setString("YES /_");
	bounds = dummyText.getGlobalBounds();
	noText.setString("NO");
	noText.setPosition(dummyText.getPosition() + sf::Vector2f(bounds.width, 0));
	yesText.setFillColor(sf::Color(255, 0, 0, 0));
	noText.setFillColor(sf::Color(255, 255, 255, 0));
}

void InterlevelContext::setAdvancingLevel() {
	if (lm.getLevelTime().getRemainingTime() <= sf::Time::Zero) {
		// No time remaining: skip this phase
		_setPromptContinue();
		return;
	}
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

void InterlevelContext::setGettingReady(unsigned short lvnum) {
	state = State::GETTING_READY;
	clock.restart();
	centralText.setString("LEVEL " + Game::to_string(lvnum));
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(Game::center(bounds, WIN_BOUNDS));
	subtitleText.setString("GET READY!");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
}

void InterlevelContext::_setPromptContinue() {
	// Check if there are dead players with continues left
	mustPromptPlayer.fill(false);
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		const auto player = lm.getPlayer(i + 1);
		if ((player == nullptr || player->get<Game::Killable>()->isKilled()) && Game::playerContinues[i] > 0) {
			mustPromptPlayer[i] = true;	
		}
	}
	unsigned short idx = 0;
	while  (!mustPromptPlayer[idx] && idx < mustPromptPlayer.size()) ++idx;
	if (idx == mustPromptPlayer.size()) {
		// all players alive or without continues: skip this phase
		setGettingReady(lm.getLevel()->getInfo().levelnum + 1);
		return;
	}
	state = State::PROMPT_CONTINUE;
	clock.restart();
	_preparePromptContinue(idx);
}

void InterlevelContext::_preparePromptContinue(unsigned short idx) {
	curPromptedPlayer = idx;
	centralText.setString("P" + Game::to_string(idx+1) + " CONTINUE? (" 
			+ Game::to_string(Game::playerContinues[idx]) + " left)");
	auto bounds = centralText.getGlobalBounds();
	centralText.setPosition(Game::center(bounds, WIN_BOUNDS));
	subtitleText.setString("/");
	bounds = subtitleText.getGlobalBounds();
	subtitleText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 2 * bounds.height));
	subsubtitleText.setString("Arrows / Enter to select");
	subsubtitleText.setCharacterSize(13);
	bounds = subsubtitleText.getGlobalBounds();
	subsubtitleText.setPosition(Game::center(bounds, WIN_BOUNDS) + sf::Vector2f(0.f, 4 * bounds.height));
	subsubtitleText.setCharacterSize(10);
	bounds = subsubtitleText.getGlobalBounds();
	subsubtitleText.setPosition(Game::centerX(bounds, WIN_BOUNDS), bounds.top);
	yesText.setFillColor(sf::Color::Red);
	yesText.setCharacterSize(15);
	noText.setFillColor(sf::Color::White);
	noText.setCharacterSize(13);
}

void InterlevelContext::update() {
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
	default:
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
			// First assignment of bonusTime: truncate decimals of remaining time
			bonusTime = sf::seconds(int(lm.getLevelTime().getRemainingTime().asSeconds()));
		} else {
			// Pass to next phase
			clock.restart();
			state = State::WAIT_DISTRIBUTING_POINTS;
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

void InterlevelContext::_tickWaitDistributePoints() {
	if (clock.getElapsedTime() > sf::seconds(2))
		_setPromptContinue();
}

void InterlevelContext::_ackPromptResponse() {
	// If player wants to continue, don't do anything here: it'll be the GameContext
	// to check if the player has spare continues, and will resurrect it if so.
	// Otherwise, zero its continues so it won't be resurrected.
	if (!yesSelected)
		Game::playerContinues[curPromptedPlayer] = 0;
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
		setGettingReady(lm.getLevel()->getInfo().levelnum + 1);
	}
}

bool InterlevelContext::handleEvent(sf::Window&, sf::Event event) {
	if (state != State::PROMPT_CONTINUE) return false;
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		// TODO
		break;
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Left:
			yesSelected = true;
			yesText.setFillColor(sf::Color::Red);
			yesText.setCharacterSize(15);
			noText.setFillColor(sf::Color::White);
			noText.setCharacterSize(13);
			return true;
		case sf::Keyboard::Right:
			yesSelected = false;
			noText.setFillColor(sf::Color::Red);
			noText.setCharacterSize(15);
			yesText.setFillColor(sf::Color::White);
			yesText.setCharacterSize(13);
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
	
void InterlevelContext::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(centralText, states);
	window.draw(subtitleText, states);
	window.draw(yesText, states);
	window.draw(noText, states);
	window.draw(subsubtitleText, states);
	window.draw(sidePanel, states);
}

void InterlevelContext::_givePoints(int amount) {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player != nullptr)
			Game::score[i] += amount;
	}
}
