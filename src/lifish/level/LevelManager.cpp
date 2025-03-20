#include "LevelManager.hpp"
#include "AlienBoss.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "Bomb.hpp"
#include "Bonusable.hpp"
#include "Coin.hpp"
#include "Controllable.hpp"
#include "Enemy.hpp"
#include "Explosion.hpp"
#include "Foe.hpp"
#include "GameCache.hpp"
#include "Letter.hpp"
#include "Level.hpp"
#include "LevelLoader.hpp"
#include "LevelSet.hpp"
#include "Lifed.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "SaveManager.hpp"
#include "Shooting.hpp"
#include "core.hpp"
#include "game_logic.hpp"
#include "utils.hpp"
#include <cassert>
#include <memory>

using lif::LevelManager;

LevelManager::LevelManager()
	: lif::BaseLevelManager()
	, renderer(*this)
	, levelTime(new lif::LevelTime)
{
	reset();
	resetPlayerPersistentData();
	for (auto logic : lif::game_logic::functions)
		logicFunctions.emplace_back(logic);
}

void LevelManager::createNewPlayers(int n) {
	for (int i = 0; i < n && i < lif::MAX_PLAYERS; ++i) {
		// Pointers kept by LevelManager
		players[i] = std::make_shared<lif::Player>(sf::Vector2f(0, 0), i + 1);
		playerContinues[i] = lif::conf::player::INITIAL_CONTINUES;
		// Pointers owned by EntityGroup
		entities.add(players[i]);
	}
}

void LevelManager::loadGame(const lif::SaveData& saveData) {
	// TODO validate saveData
	for (unsigned i = 0; i < std::min({
		static_cast<unsigned>(saveData.players.size()),
		static_cast<unsigned>(lif::MAX_PLAYERS),
		static_cast<unsigned>(lif::options.nPlayers)
	}); ++i)
	{
		const auto& pdata = saveData.players[i];
		auto& player = players[i];
		// Continues
		playerContinues[i] = pdata.continues;

		if (player.get() == nullptr)
			continue;

		if (playerContinues[i] < 0) {
			player->setRemainingLives(0);
			player->get<lif::Lifed>()->setLife(0);
			player->get<lif::Animated>()->setActive(false);
			player->get<lif::Killable>()->kill();
		} else {
			player->setRemainingLives(pdata.remainingLives);
			player->get<lif::Lifed>()->setLife(pdata.life);
		}
		// Powers
		auto& powers = player->getPowers();
		powers.bombFuseTime = pdata.powers.bombFuseTime;
		powers.bombRadius = pdata.powers.bombRadius;
		powers.maxBombs = pdata.powers.maxBombs;
		// Letters
		for (unsigned j = 0; j < lif::conf::player::N_EXTRA_LETTERS; ++j)
			player->setExtra(j, pdata.letters[j]);
		// Score
		score[i] = pdata.score;
	}
}

void LevelManager::setPlayer(int id, std::shared_ptr<lif::Player> player) {
	players[id - 1].swap(player);
}

void LevelManager::removePlayer(int id) {
	players[id - 1].reset();
}

void LevelManager::update() {
	DBGSTART("lmtot");
	BaseLevelManager::update();
	teleportSystem.update();

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

const std::shared_ptr<lif::Player> LevelManager::getPlayer(int id) const {
	return players[id-1];
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	renderer.draw(target, states);
}

void LevelManager::setNextLevel() {
	if (level == nullptr)
		throw std::logic_error("Called LevelManager::setNextLevel() with null level!");
	short lvnum = level->getInfo().levelnum + 1;
	if (lvnum > level->getLevelSet().getLevelsNum())
		lvnum = 1;
	setLevel(level->getLevelSet(), lvnum);
}

void LevelManager::setLevel(const lif::LevelSet& ls, int lvnum) {
	level = ls.getLevel(lvnum);
	const auto lvinfo = level->getInfo();
	lif::LevelLoader::load(*level, *this);
	cd.setLevelLimit(sf::FloatRect(lif::TILE_SIZE, lif::TILE_SIZE,
				(lvinfo.width + 1) * lif::TILE_SIZE,
				(lvinfo.height + 1) * lif::TILE_SIZE));
	// Don't trigger EXTRA game if there were no coins in the level
	if (entities.size<lif::Coin>() == 0)
		extraGameTriggered = true;
}

void LevelManager::reset() {
	BaseLevelManager::reset();

	teleportSystem.clear();

	dropTextManager.reset();
	// Re-add the dropping texts
	dropTextManager.subscribe(entities);
	// Re-add the level time
	entities.add(levelTime);

	hurryUp = hurryUpWarningGiven = false;
	extraGameTriggered = extraGame = false;
	gameOver = false;
}

void LevelManager::resetPlayerPersistentData() {
	score.fill(0);
	playerContinues.fill(0);
}

bool LevelManager::canDeployBomb(const lif::Player& player) const {
	const auto pinfo = player.getInfo();
	const bool mostlyAligned = lif::manhattanDistance(
			player.getPosition(), lif::aligned2(player.getPosition())) < 4;
	return mostlyAligned && bombsDeployedBy(pinfo.id) < pinfo.powers.maxBombs;
}

bool LevelManager::canDeployBombAt(const sf::Vector2i& tile) const {
	if (_isBombAt(tile)) return false;
	for (const auto cld : entities.getCollidersIntersecting(sf::FloatRect(
			lif::TILE_SIZE * tile.x,
			lif::TILE_SIZE * tile.y,
			lif::TILE_SIZE, lif::TILE_SIZE)))
	{
		if (cld->getLayer() == lif::c_layers::EXPLOSIONS)
			return false;
	}
	return true;
}

bool LevelManager::_isBombAt(const sf::Vector2i& tile) const {
	for (unsigned i = 0; i < bombs.size(); ++i)
		for (auto bomb : bombs[i])
			if (!bomb.expired() && lif::tile(bomb.lock()->getPosition()) == tile)
				return true;
	return false;
}

int LevelManager::bombsDeployedBy(int id) const {
	return std::count_if(bombs[id-1].begin(), bombs[id-1].end(), [] (std::weak_ptr<lif::Bomb> b) {
		return !b.expired();
	});
}

lif::Bomb* LevelManager::getFirstValidBomb(int id) const {
	const auto& pbombs = bombs[id - 1];
	for (unsigned i = 0; i < pbombs.size(); ++i) {
		if (!pbombs[i].expired()) {
			const auto& bomb = pbombs[i].lock();
			if (bomb != nullptr)
				return bomb.get();
		}
	}
	return nullptr;
}

bool LevelManager::isLevelClear() const {
	bool clear = true;
	entities.apply([&clear] (const lif::Entity& e) {
		if (e.get<lif::Foe>() != nullptr) {
			clear = false;
			return lif::EntityGroup::APPLY_EXIT;
		}
		return lif::EntityGroup::APPLY_PROCEED;
	});
	return clear;
}

void LevelManager::_spawn(lif::Entity *e) {
	if (auto b = dynamic_cast<lif::Bomb*>(e))
		_spawnBomb(b);
	else
		lif::BaseLevelManager::_spawn(e);
}

void LevelManager::_spawnBomb(lif::Bomb *b) {
	const auto player = dynamic_cast<const lif::Player*>(b->getSourceEntity());
	if (player == nullptr) {
		// Bomb was not spawned by a player
		entities.add(b);
		return;
	}
	const auto id = player->getInfo().id - 1;
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
	lif::cache.playSound(lif::getAsset("sounds", lif::HURRY_UP_SOUND));
	hurryUpWarningGiven = true;
}

void LevelManager::_triggerHurryUp() {
	entities.apply([] (lif::Entity& e) {
		if (auto enemy = dynamic_cast<lif::Enemy*>(&e)) {
			auto moving = enemy->get<lif::Moving>();
			moving->setSpeed(moving->getOriginalSpeed() * 2);
			enemy->get<lif::Shooting>()->setFireRateMult(2);
		} else if (auto boss = dynamic_cast<lif::AlienBoss*>(&e)) {
			boss->setShootIntervalMul(0.5f);
		}
	});
	hurryUp = true;
}

void LevelManager::_triggerExtraGame() {
	entities.apply([] (lif::Entity& e) {
		auto enemy = dynamic_cast<lif::Enemy*>(&e);
		if (enemy == nullptr) return;
		if (enemy == nullptr || enemy->get<lif::Killable>()->isKilled()) return;

		enemy->setMorphed(true);
	});
	dropTextManager.trigger(lif::DroppingTextManager::Text::EXTRA_GAME);
	lif::cache.playSound(lif::getAsset("sounds", lif::EXTRA_GAME_SOUND));
	levelTime->startExtraGame();
	extraGameTriggered = extraGame = true;
}

void LevelManager::_endExtraGame() {
	entities.apply([] (lif::Entity& e) {
		if (auto letter = dynamic_cast<lif::Letter*>(&e)) {
			letter->get<lif::Killable>()->kill();
			return;
		}

		auto enemy = dynamic_cast<lif::Enemy*>(&e);
		if (enemy == nullptr) return;

		enemy->setMorphed(false);
	});

	extraGame = false;
}

bool LevelManager::_shouldTriggerExtraGame() const {
	bool thereAreCoins = false;
	entities.apply([&thereAreCoins] (const lif::Entity& e) {
		auto coin = dynamic_cast<const lif::Coin*>(&e);
		if (coin == nullptr) return lif::EntityGroup::APPLY_PROCEED;

		if (!coin->get<lif::Killable>()->isKilled()) {
			thereAreCoins = true;
			return lif::EntityGroup::APPLY_EXIT;
		}

		return lif::EntityGroup::APPLY_PROCEED;
	});

	return !thereAreCoins;
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
		const auto& player = players[i];
		if (player == nullptr)
			continue;

		auto klb = player->get<lif::Killable>();
		if (klb->isKilled() && !klb->isKillInProgress()) {
			if (_tryResurrectPlayer(player))
				++living_players;
		} else {
			++living_players;
		}
	}

	// No players alive: if any player has at least 1 continue, retry level
	// else, game over.
	if (living_players == 0) {
		for (auto cont : playerContinues) {
			if (cont > 0) {
				mustRetry = true;
				return;
			}
		}
		gameOver = true;
	}
}

bool LevelManager::_tryResurrectPlayer(const std::shared_ptr<lif::Player>& player) {
	if (player == nullptr)
		return false;
	if (player->getInfo().remainingLives > 0) {
		player->resurrect();
		player->get<lif::Bonusable>()->giveBonus(lif::BonusType::SHIELD,
		                                         lif::conf::player::RESURRECT_SHIELD_TIME);
		player->setRemainingLives(player->getInfo().remainingLives - 1);
		// Re-add player after entitygroup automatically removed it
		entities.checkAll();
		entities.add(player);
		return true;
	} else {
		players[player->getInfo().id - 1] = nullptr;
		return false;
	}
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

	// Check level bounds
	if (iposx <= 0 || iposx > level->getInfo().width || iposy <= 0 || iposy > level->getInfo().height)
		return false;

	// If the entity has no collider, it can always move within the level
	const auto collider = am.getOwner().get<lif::Collider>();
	if (collider == nullptr)
		return true;

	// Check if another collider blocks the way
	for (const auto cld : entities.getCollidersIntersecting(sf::FloatRect(
			iposx * TILE_SIZE, iposy * TILE_SIZE, TILE_SIZE, TILE_SIZE)))
	{
		if (cld->getOwner().get<lif::Fixed>() == nullptr)
			continue;

		if (collider->isSolidFor(*cld))
			return false;
	}

	return true;
}


int LevelManager::addScore(int id, int amt) {
	assert(0 < id && id <= lif::MAX_PLAYERS);
	if (players[id - 1])
		score[id - 1] = std::max(0, score[id - 1] + amt);
	return score[id - 1];
}

void LevelManager::addScoreToAll(int amt) {
	for (auto i = 0u; i < players.size(); ++i)
		if (players[i])
			addScore(i + 1, amt);
}

int LevelManager::getScore(int id) const {
	assert(0 < id && id <= lif::MAX_PLAYERS);
	return score[id - 1];
}

short LevelManager::getPlayerContinues(int id) const {
	assert(0 < id && id <= lif::MAX_PLAYERS);
	return playerContinues[id - 1];
}

void LevelManager::decPlayerContinues(int id) {
	assert(0 < id && id <= lif::MAX_PLAYERS);
	--playerContinues[id - 1];
}

void LevelManager::setPlayerContinues(int id, int amt) {
	assert(0 < id && id <= lif::MAX_PLAYERS);
	playerContinues[id - 1] = amt;
}

void LevelManager::resetLevel() {
	mustRetry = false;
	setLevel(level->getLevelSet(), level->getInfo().levelnum);
}
