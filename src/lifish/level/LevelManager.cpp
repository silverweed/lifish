#include "LevelManager.hpp"
#include "game_logic.hpp"
#include "Enemy.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"
#include "Explosion.hpp"
#include "Foe.hpp"
#include "Music.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "SaveManager.hpp"
#include "Player.hpp"
#include "Bomb.hpp"
#include "Level.hpp"
#include "Shooting.hpp"
#include "Letter.hpp"
#include "LevelLoader.hpp"
#include "Coin.hpp"
#include "Bonusable.hpp"
#include "Controllable.hpp"
#include "LevelSet.hpp"
#include "core.hpp"
#include <memory>

using lif::LevelManager;

LevelManager::LevelManager()
	: lif::BaseLevelManager(sf::FloatRect(
		lif::TILE_SIZE,
		lif::TILE_SIZE,
		(lif::LEVEL_WIDTH + 1) * lif::TILE_SIZE,
		(lif::LEVEL_HEIGHT + 1) * lif::TILE_SIZE))
	, renderer(*this)
	, effects(sf::Vector2u(lif::LEVEL_WIDTH * lif::TILE_SIZE, lif::LEVEL_HEIGHT * lif::TILE_SIZE))
	, levelTime(new lif::LevelTime)
{
	reset();
	for (auto logic : lif::game_logic::functions)
		logicFunctions.push_back(logic);
}

void LevelManager::createNewPlayers(unsigned short n) {
	for (int i = 0; i < n && i < lif::MAX_PLAYERS; ++i) {
		// Pointers kept by LevelManager
		players[i] = std::make_shared<lif::Player>(sf::Vector2f(0, 0), i + 1);
		// Pointers owned by EntityGroup
		entities.add(players[i]);
	}
}

void LevelManager::setPlayer(unsigned short id, std::shared_ptr<lif::Player> player) {
	players[id - 1].swap(player);
}

void LevelManager::removePlayer(unsigned short id) {
	players[id - 1].reset();
}

void LevelManager::update() {
	DBGSTART("lmtot");
	BaseLevelManager::update();

	DBGSTART("checks");

	// Check if should resurrect players
	if (!gameOver)
		_checkResurrect();

	// Hurry up, extra game
	_checkSpecialConditions();

	DBGEND("checks");
	DBGEND("lmtot");
}

bool LevelManager::isPlayer(const lif::Entity& e) const {
	for (const auto& p : players)
		if (&e == p.get()) return true;
	return false;
}

const std::shared_ptr<lif::Player> LevelManager::getPlayer(unsigned short id) const {
	return players[id-1];
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	renderer.draw(target, states);
}

void LevelManager::setNextLevel() {
	if (level == nullptr)
		throw std::logic_error("Called LevelManager::setNextLevel() with null level!");
	short lvnum = level->getInfo().levelnum + 1;
	if (lvnum >= level->getLevelSet().getLevelsNum())
		lvnum = 1;
	setLevel(level->getLevelSet(), lvnum);
}

void LevelManager::setLevel(const lif::LevelSet& ls, unsigned short lvnum) {
	_mtxLock();
	level = ls.getLevel(lvnum);
	effects.setEffects(level->getInfo().effects);
	lif::LevelLoader::load(*level, *this);
	_mtxUnlock();
	// Don't trigger EXTRA game if there were no coins in the level
	if (entities.size<lif::Coin>() == 0)
		extraGameTriggered = true;
}

void LevelManager::reset() {
	BaseLevelManager::reset();

	dropTextManager.reset();
	// Re-add the dropping texts
	dropTextManager.subscribe(entities);
	// Re-add the level time
	entities.add(levelTime);

	hurryUp = hurryUpWarningGiven = false;
	extraGameTriggered = extraGame = false;
	gameOver = false;
}

bool LevelManager::canDeployBombAt(const sf::Vector2i& tile) const {
	if (_isBombAt(tile)) return false;
	bool there_are_expl = false;
	entities.apply([tile, &there_are_expl] (const lif::Entity *e) {
		if (there_are_expl) return;
		if (lif::tile(e->getPosition()) == tile && dynamic_cast<const lif::Explosion*>(e) != nullptr)
			there_are_expl = true;
	});
	return !there_are_expl;
}

bool LevelManager::_isBombAt(const sf::Vector2i& tile) const {
	for (unsigned i = 0; i < bombs.size(); ++i)
		for (auto bomb : bombs[i])
			if (!bomb.expired() && lif::tile(bomb.lock()->getPosition()) == tile)
				return true;
	return false;
}

unsigned short LevelManager::bombsDeployedBy(unsigned short id) const {
	return std::count_if(bombs[id-1].begin(), bombs[id-1].end(), [] (std::weak_ptr<lif::Bomb> b) {
		return !b.expired();
	});
}

bool LevelManager::isLevelClear() const {
	bool clear = true;
	entities.apply([&clear] (const lif::Entity *e) {
		if (clear && e->get<lif::Foe>() != nullptr)
			clear = false;
	});
	return clear;
}

void LevelManager::_spawn(lif::Entity *e) {
	if (auto b = dynamic_cast<lif::Bomb*>(e))
		_spawnBomb(b);
	else
		entities.add(e);
}

void LevelManager::_spawnBomb(lif::Bomb *b) {
	const auto id = b->getSourcePlayer().getInfo().id - 1;
	// Spawn bomb only if player has not deployed all the available ones already
	for (unsigned i = 0; i < bombs[id].size(); ++i) {
		if (bombs[id][i].expired()) {
			std::shared_ptr<lif::Bomb> bomb(b);
			entities.add(bomb);
			bombs[id][i] = bomb;
			break;
		}
	}
}

void LevelManager::_triggerHurryUpWarning() {
	dropTextManager.trigger(lif::DroppingTextManager::Text::HURRY_UP);
	hurryUpWarningGiven = true;
}

void LevelManager::_triggerHurryUp() {
	entities.apply([] (lif::Entity *e) {
		auto enemy = dynamic_cast<lif::Enemy*>(e);
		if (enemy == nullptr) return;

		auto moving = enemy->get<lif::Moving>();
		moving->setSpeed(moving->getOriginalSpeed() * 2);
		enemy->get<lif::Shooting>()->setFireRateMult(2);
	});
	hurryUp = true;
}

void LevelManager::_triggerExtraGame() {
	entities.apply([] (lif::Entity *e) {
		auto enemy = dynamic_cast<lif::Enemy*>(e);
		if (enemy == nullptr) return;
		
		enemy->setMorphed(true);
	});
	dropTextManager.trigger(lif::DroppingTextManager::Text::EXTRA_GAME);
	levelTime->startExtraGame();
	extraGameTriggered = extraGame = true;
}

void LevelManager::_endExtraGame() {
	entities.apply([] (lif::Entity *e) {
		if (auto letter = dynamic_cast<lif::Letter*>(e)) {
			letter->get<lif::Killable>()->kill();
			return;
		}

		auto enemy = dynamic_cast<lif::Enemy*>(e);
		if (enemy == nullptr) return;
		
		enemy->setMorphed(false);
	});
	
	extraGame = false;
}

bool LevelManager::_shouldTriggerExtraGame() const {
	bool there_are_coins = false;

	entities.apply([&there_are_coins] (const lif::Entity *e) {
		if (there_are_coins) return;

		auto coin = dynamic_cast<const lif::Coin*>(e);
		if (coin == nullptr) return;

		if (!coin->get<lif::Killable>()->isKilled()) {
			there_are_coins = true;
			return;
		}
	});

	return !there_are_coins;
}

void LevelManager::_checkSpecialConditions() {
	if (!hurryUpWarningGiven && levelTime->checkHurryUp() == lif::LevelTime::HurryUpResponse::HURRY_UP_NEAR)
		_triggerHurryUpWarning();
	else if (!hurryUp && levelTime->checkHurryUp() == lif::LevelTime::HurryUpResponse::HURRY_UP_ON)
		_triggerHurryUp();

	if (!extraGameTriggered && _shouldTriggerExtraGame())
		_triggerExtraGame();
	else if (extraGame && levelTime->getRemainingExtraGameTime() <= sf::Time::Zero)
		_endExtraGame();
}

void LevelManager::_checkResurrect() {
	unsigned living_players = 0;

	for (unsigned i = 0; i < players.size(); ++i) {
		auto player = players[i];
		if (player == nullptr)
			continue;

		auto klb = player->get<lif::Killable>();
		if (klb->isKilled() && !klb->isKillInProgress()) {
			if (player->getInfo().remainingLives > 0) {
				player->resurrect();
				player->get<lif::Bonusable>()->giveBonus(lif::BonusType::SHIELD,
				                                          lif::conf::player::RESURRECT_SHIELD_TIME);
				player->setRemainingLives(player->getInfo().remainingLives - 1);
				entities.add(player);
				++living_players;
			} else {
				entities.remove(player);
				players[i] = nullptr;
			}
		} else {
			++living_players;
		}
	}

	if (living_players == 0)
		gameOver = true;
}

bool LevelManager::canGo(const lif::AxisMoving& am, const lif::Direction dir) const {
	auto pos = am.getOwner().getPosition();
	int iposx = static_cast<int>(pos.x / TILE_SIZE),
	    iposy = static_cast<int>(pos.y / TILE_SIZE);
	
	switch (dir) {
	case Direction::UP:
		--iposy;
		break;
	case Direction::LEFT:
		--iposx;
		break;
	case Direction::DOWN:
		++iposy;
		break;
	case Direction::RIGHT:
		++iposx;
		break;
	default:
		return true;
	}

	if (iposx <= 0 || iposx > LEVEL_WIDTH || iposy <= 0 || iposy > LEVEL_HEIGHT)
		return false;

	const auto collider = am.getOwner().get<lif::Collider>();
	if (collider == nullptr)
		return true;

	for (auto ptr : entities.getEntitiesAtTile(sf::Vector2i(iposx, iposy))) {
		auto e = ptr.lock();
		if (e->get<lif::Fixed>() == nullptr) continue;
		const auto fcollider = e->get<lif::Collider>();
		if (fcollider != nullptr && collider->isSolidFor(*fcollider))
			return false;
	}

	return true;
}

