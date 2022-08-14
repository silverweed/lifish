#pragma once

#include "BaseLevelManager.hpp"
#include "Direction.hpp"
#include "DroppingTextManager.hpp"
#include "LevelRenderer.hpp"
#include "LevelTime.hpp"
#include "TeleportSystem.hpp"
#include "bonus_type.hpp"
#include "conf/player.hpp"
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <array>

namespace lif {

struct SaveData;
class LevelLoader;
class LevelSet;
class SaveManager;
class WinLoseHandler;
class Player;
class Bomb;
class Level;
class AxisMoving;

#ifndef RELEASE
namespace debug {
class DebugEventHandler;
}
#endif

/** The class that manages all the level's entities and events.
 *  In particular, its update() method updates all entities,
 *  the collisions and the game logic.
 */
class LevelManager final : public lif::BaseLevelManager, public sf::Drawable {

	friend class lif::LevelLoader;
	friend class lif::LevelRenderer;
	friend class lif::SaveManager;
	friend class lif::WinLoseHandler;
	friend void lif::triggerBonus(lif::LevelManager& lm, lif::BonusType type, lif::Player& player);
#ifndef RELEASE
	friend class lif::debug::DebugEventHandler;
#endif

	/** The currently managed level */
	std::unique_ptr<lif::Level> level;
	lif::LevelRenderer renderer;
	std::shared_ptr<lif::LevelTime> levelTime;
	lif::TeleportSystem teleportSystem;

	/** Whether hurry up has already been triggered or not */
	bool hurryUp = false;
	bool hurryUpWarningGiven = false;
	/** Whether EXTRA game was already triggered or not */
	bool extraGameTriggered = false;
	/** Whether we're currently in EXTRA game or not */
	bool extraGame = false;
	/** This is set to `true` as soon as no players are found alive (and they have no continue). */
	bool gameOver = false;
	/** This is set to `true` if both player have 0 lives but some continues remaining */
	bool mustRetry = false;
	/** This is set to `true` when the last level is completed */
	bool gameWon = false;

	lif::DroppingTextManager dropTextManager;

	/** "Owned" pointers to players.
	 *  The players' lifecycle is the following:
	 *  1) players are created via `createNewPlayers` and added to `entities`; the LevelManager and the
	 *     EntityGroup share the ownership of the players via shared_ptr;
	 *  2) updates to players are managed by the EntityGroup;
	 *  3) on level change, `entities` gets cleared; the player entities survive, as LevelManager retains
	 *     their pointers, so the RC doesn't drop to 0;
	 *  5) the players are then readded to the EntityGroup.
	 */
	std::array<std::shared_ptr<lif::Player>, lif::MAX_PLAYERS> players;
	/** The players' score */
	std::array<int, MAX_PLAYERS> score;
	/** The remaining 'continues' per player */
	std::array<short, MAX_PLAYERS> playerContinues;


	/** Unowned references to bombs currently deployed by players */
	lif::Matrix<std::weak_ptr<lif::Bomb>, lif::MAX_PLAYERS, lif::conf::player::MAX_MAX_BOMBS> bombs;


	/** Adds the given entity to `entities` */
	void _spawn(lif::Entity *e) override;
	void _spawnBomb(lif::Bomb *b);
	/** Checks if there are any player to resurrect. If no players are found alive,
	 *  sets the `gameOver` flag to `true`.
	 */
	void _checkResurrect();
	/** If player has continues left, resurrect it and return true.
	 *  Else, remove it from `players` and return false.
	 */
	bool _tryResurrectPlayer(const std::shared_ptr<lif::Player>& player);
	void _checkSpecialConditions();
	void _triggerHurryUp();
	void _triggerHurryUpWarning();
	void _triggerExtraGame();
	void _endExtraGame();
	bool _shouldTriggerExtraGame() const;
	bool _isBombAt(const sf::Vector2i& tile) const;

public:
	explicit LevelManager();

	/** Generates n players. If n > MAX_PLAYERS, only generate MAX_PLAYERS players. */
	void createNewPlayers(int n = lif::MAX_PLAYERS);

	bool isPlayer(const lif::Entity& e) const;
	/** Returns the id-th player (id starting from 1) */
	const std::shared_ptr<lif::Player> getPlayer(int id) const;
	void setPlayer(int id, std::shared_ptr<lif::Player> player);
	void removePlayer(int id);

	/** Adds `amt` points to player `id`'s score and returns new score (id starts from 1) */
	int addScore(int id, int amt);
	void addScoreToAll(int amt);
	int getScore(int id) const;

	short getPlayerContinues(int id) const;
	void setPlayerContinues(int id, int amt);
	void decPlayerContinues(int id);

	void loadGame(const lif::SaveData& saveData);

	const lif::Level* getLevel() const { return level != nullptr ? level.get() : nullptr; }
	/** Loads `lvnum`-th level from `ls` into this LevelManager. Destroys previous level if any */
	void setLevel(const lif::LevelSet& ls, int lvnum);
	/** Loads next level from `level->getLevelSet()`. Throws if `level` is currently null */
	void setNextLevel();
	void resetLevel();

	const lif::LevelTime& getLevelTime() const { return *levelTime; }

	/** @return the game over state. The game is over when all players have 0 life and 0 continues. */
	bool isGameOver() const { return gameOver; }
	/** @return whether the current level is clear of enemies (entities marked with 'Foe'). */
	bool isLevelClear() const;
	bool isGameWon() const { return gameWon; }
	bool mustRetryLevel() const { return mustRetry; }

	/** @return whether the owner of `am` can proceed along direction `dir` */
	bool canGo(const lif::AxisMoving& am, const lif::Direction dir) const;
	/** @return whether a player can deploy a bomb or not */
	bool canDeployBomb(const lif::Player& player) const;
	/** @return whether a player can deploy a bomb in a specific tile.
	 *  A bomb can be deployed in a tile only if no other bomb or explosion is currently in that tile.
	 */
	bool canDeployBombAt(const sf::Vector2i& tile) const;
	/** @return the number of bombs currently deployed by id-th player */
	int bombsDeployedBy(int id) const;
	/** @return the first valid bomb deployed by id-th player, or nullptr if there are none */
	lif::Bomb* getFirstValidBomb(int id) const;

	bool entityJustTeleported(const lif::Entity& entity) const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update() override;
	void reset() override;
	/** Resets player continues and score */
	void resetPlayerPersistentData();
};

}
