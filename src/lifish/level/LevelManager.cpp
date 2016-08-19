#include "LevelManager.hpp"
#include "game_logic.hpp"
#include "Enemy.hpp"
#include <memory>
//#include <iostream>

using Game::LevelManager;

LevelManager::LevelManager() 
	: renderer(*this)
	, cd(entities)
{
	levelTime.init();
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

	// If time is up, trigger HURRY UP
	if (!hurryUp && levelTime.checkHurryUp() == Game::LevelTime::HurryUpResponse::HURRY_UP_ON)
		_triggerHurryUp();
}

bool LevelManager::isPlayer(const Game::Entity *const e) const {
	for (const auto& p : players)
		if (e == p.get()) return true;
	return false;
}

const Game::Player* LevelManager::getPlayer(unsigned short id) const {
	return players[id-1].get();
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	renderer.draw(target, states);
}

void LevelManager::setOrigin(const sf::Vector2f& pos) {
	renderer.setOrigin(pos);
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
	auto b = dynamic_cast<Game::Bomb*>(e);
	if (b != nullptr)
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
