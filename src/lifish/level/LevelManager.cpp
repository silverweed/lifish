#include "LevelManager.hpp"
#include "game_logic.hpp"
#include "Enemy.hpp"
#include "AxisMoving.hpp"
#include "SaveManager.hpp"
#include "Player.hpp"
#include "Bomb.hpp"
#include "Level.hpp"
#include "Shooting.hpp"
#include "Letter.hpp"
#include "LevelLoader.hpp"
#include "Coin.hpp"
#include "Bonusable.hpp"
#include <memory>
#include <iostream>

using Game::LevelManager;

LevelManager::LevelManager() 
	: renderer(*this)
	, cd(entities)
{
	levelTime.init();
	dropTextManager.subscribe(entities);
}

auto LevelManager::createNewPlayers(unsigned short n) -> std::vector<Game::Player*> {
	std::vector<Game::Player*> pls;
	for (int i = 0; i < n && i < Game::MAX_PLAYERS; ++i) {
		// Pointers kept by LevelManager
		players[i] = std::make_shared<Game::Player>(sf::Vector2f(0, 0), i + 1);
		// Pointers owned by EntityGroup
		entities.add(players[i]);
		// Returned (unowned) pointers
		pls.push_back(players[i].get());
	}
	return pls;
}

void LevelManager::update() {
	// Update level time
	levelTime.update();

	std::list<Game::Entity*> to_be_spawned, to_be_killed;

	// Set prevAligns for aligned entities
	entities.apply([] (Game::Entity *e) {
		if (!e->isAligned()) return;
		auto moving = e->get<Game::AxisMoving>();
		if (moving == nullptr) return;
		moving->setPrevAlign(Game::tile(e->getPosition()));
	});

	// Update collisions
	cd.update();

	// Apply game logic rules
	for (auto logic : Game::Logic::functions)
		entities.apply(logic, *this, to_be_spawned, to_be_killed);

	for (auto e : to_be_spawned)
		spawn(e);

	for (auto e : to_be_killed)
		entities.remove(*e);

	// Update entities and their components
	entities.updateAll();

	// Check if should resurrect players
	_checkResurrect();

	// Hurry up, extra game
	_checkSpecialConditions();
}

bool LevelManager::isPlayer(const Game::Entity& e) const {
	for (const auto& p : players)
		if (&e == p.get()) return true;
	return false;
}

const Game::Player* LevelManager::getPlayer(unsigned short id) const {
	return players[id-1].get();
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	renderer.draw(target, states);
}

void LevelManager::setOrigin(const sf::Vector2f& pos) {
	if (level == nullptr) return;
	level->setOrigin(pos);
	entities.setOrigin(pos);
}

void LevelManager::setLevel(Game::Level& lv) {
	level = &lv;
	Game::LevelLoader::load(lv, *this);
	// Don't trigger EXTRA game if there were no coins in the level
	if (entities.size<Game::Coin>() == 0)
		extraGameTriggered = true;
}

void LevelManager::pause() {
	levelTime.pause();
	entities.apply([] (Game::Entity *e) {
		auto clocks = e->getAllRecursive<Game::Clock>();
		for (auto clock : clocks)
			clock->pause();
	});
	paused = true;
}

void LevelManager::resume() {
	levelTime.resume();
	entities.apply([] (Game::Entity *e) {
		auto clocks = e->getAllRecursive<Game::Clock>();
		for (auto clock : clocks)
			clock->resume();
	});
	paused = false;
}

void LevelManager::reset() {
	entities.clear();
	// Re-add the dropping texts
	dropTextManager.subscribe(entities);

	hurryUp = hurryUpWarningGiven = false;
	extraGameTriggered = extraGame = false;
}

bool LevelManager::isBombAt(const sf::Vector2i& tile) const {
	for (unsigned short i = 0; i < bombs.size(); ++i)
		for (auto bomb : bombs[i])
			if (!bomb.expired() && Game::tile(bomb.lock()->getPosition()) == tile)
				return true;
	return false;
}

unsigned short LevelManager::bombsDeployedBy(unsigned short id) const {
	return std::count_if(bombs[id-1].begin(), bombs[id-1].end(), [] (const std::weak_ptr<Game::Bomb>& b) {
		return !b.expired();
	});
}

void LevelManager::spawn(Game::Entity *e) {
	if (auto b = dynamic_cast<Game::Bomb*>(e))
		_spawnBomb(b);
	else
		entities.add(e);
}

void LevelManager::_spawnBomb(Game::Bomb *b) {
	const auto id = b->getSourcePlayer().getInfo().id - 1;
	// Spawn bomb only if player has not deployed all the available ones already
	for (unsigned short i = 0; i < bombs[id].size(); ++i) {
		if (bombs[id][i].expired()) {
			std::shared_ptr<Game::Bomb> bomb(b);
			entities.add(bomb);
			bombs[id][i] = bomb;
			break;
		}
	}
}

void LevelManager::_triggerHurryUpWarning() {
	dropTextManager.trigger(Game::DroppingTextManager::Text::HURRY_UP);
	hurryUpWarningGiven = true;
}

void LevelManager::_triggerHurryUp() {
	entities.apply([] (Game::Entity *e) {
		auto enemy = dynamic_cast<Game::Enemy*>(e);
		if (enemy == nullptr) return;

		auto moving = enemy->get<Game::Moving>();
		moving->setSpeed(moving->getOriginalSpeed() * 2);
		enemy->get<Game::Shooting>()->setFireRateMult(2);
	});
	hurryUp = true;
}

void LevelManager::_triggerExtraGame() {
	entities.apply([] (Game::Entity *e) {
		auto enemy = dynamic_cast<Game::Enemy*>(e);
		if (enemy == nullptr) return;
		
		enemy->setMorphed(true);
	});
	dropTextManager.trigger(Game::DroppingTextManager::Text::EXTRA_GAME);
	levelTime.startExtraGame();
	extraGameTriggered = extraGame = true;
}

void LevelManager::_endExtraGame() {
	entities.apply([] (Game::Entity *e) {
		if (auto letter = dynamic_cast<Game::Letter*>(e)) {
			letter->get<Game::Killable>()->kill();
			return;
		}

		auto enemy = dynamic_cast<Game::Enemy*>(e);
		if (enemy == nullptr) return;
		
		enemy->setMorphed(false);
	});
	
	extraGame = false;
}

bool LevelManager::_shouldTriggerExtraGame() const {
	bool there_are_coins = false;

	entities.apply([&there_are_coins] (const Game::Entity *e) {
		if (there_are_coins) return;

		auto coin = dynamic_cast<const Game::Coin*>(e);
		if (coin == nullptr) return;

		if (!coin->get<Game::Killable>()->isKilled()) {
			there_are_coins = true;
			return;
		}
	});

	return !there_are_coins;
}

void LevelManager::_checkSpecialConditions() {
	if (!hurryUpWarningGiven && levelTime.checkHurryUp() == Game::LevelTime::HurryUpResponse::HURRY_UP_NEAR)
		_triggerHurryUpWarning();
	else if (!hurryUp && levelTime.checkHurryUp() == Game::LevelTime::HurryUpResponse::HURRY_UP_ON)
		_triggerHurryUp();

	if (!extraGameTriggered && _shouldTriggerExtraGame()) 
		_triggerExtraGame();
	else if (extraGame && levelTime.getRemainingExtraGameTime() <= sf::Time::Zero)
		_endExtraGame();
}

void LevelManager::_checkResurrect() {
	for (auto& player : players) {
		auto klb = player->get<Game::Killable>();
		if (klb->isKilled() && !klb->isKillInProgress()) {
			if (player->getInfo().remainingLives > 0) {
				player->resurrect();
				player->get<Game::Bonusable>()->giveBonus(Game::BonusType::SHIELD, 
						Game::Conf::Player::RESURRECT_SHIELD_TIME);
				player->setRemainingLives(player->getInfo().remainingLives - 1);
				entities.add(player);
			}
		}
	}
}
