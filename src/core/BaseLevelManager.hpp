#pragma once

#include "EntityGroup.hpp"
#include "SHCollisionDetector.hpp"
#include <SFML/System/NonCopyable.hpp>
#ifndef RELEASE
#	include "Stats.hpp"
#	define DBGSTART(name) \
		dbgStats.timer.start(name)
#	define DBGEND(name) \
		dbgStats.timer.end(name)
#else
#	define DBGSTART(name)
#	define DBGEND(name)
#endif

namespace lif {

class BaseLevelManager : private sf::NonCopyable {
public:
	using GameLogicFunc = std::function<void(
		lif::Entity&,              // for-each loop variable
		lif::BaseLevelManager&,    // LevelManager calling this function
		std::vector<lif::Entity*>& // vector of entities to be spawned after calling game logic
	)>;

protected:
	lif::EntityGroup entities;
	lif::SHCollisionDetector cd;

	std::vector<GameLogicFunc> logicFunctions;

	bool paused = false;

	sf::Time inputDisableTime = sf::Time::Zero;
	sf::Clock inputDisableClock;

#ifndef RELEASE
	lif::debug::Stats dbgStats;
#endif

	virtual void _spawn(lif::Entity *e);
public:
	explicit BaseLevelManager();
	virtual ~BaseLevelManager() {}

	const lif::EntityGroup& getEntities() const { return entities; }
	lif::EntityGroup& getEntities() { return entities; }
	const lif::CollisionDetector& getCollisionDetector() const { return cd; }

	/** Pauses all Clock components of all entities */
	virtual void pause();
	/** Resumes all Clock components of all entities */
	virtual void resume();
	bool isPaused() const { return paused; }

	void disableInputFor(const sf::Time& time);
	bool isInputDisabled() const;

	/** Updates all entities and collisions */
	virtual void update();

	/** Clears `entities` and resets internal variables */
	virtual void reset();

#ifndef RELEASE
	const lif::debug::Stats& getStats() const { return dbgStats; }
#endif
};

}
