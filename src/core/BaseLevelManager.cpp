#include "BaseLevelManager.hpp"
#include "AxisMoving.hpp"
#include "Clock.hpp"
#include "Time.hpp"
#include "Bomb.hpp"
#include "game.hpp"
#include <optional>
#include "DebugPainter.hpp"
#include "BombFormationsManager.hpp"

using lif::BaseLevelManager;

BaseLevelManager::BaseLevelManager()
	: cd(entities)
{}

void BaseLevelManager::update() {
	DBGSTART("tot");
	DBGSTART("reset_align");

	// Set prevAligns for aligned entities
	entities.apply([] (lif::Entity& e) {
		if (!e.isAligned()) return;
		auto moving = e.get<lif::AxisMoving>();
		if (moving == nullptr) return;
		moving->setPrevAlign(lif::tile(e.getPosition()));
	});

	DBGEND("reset_align");
	DBGSTART("validate");

	// Force pruning of all expired pointers
	entities.validate();

	DBGEND("validate");
	DBGSTART("cd");

	// Calculate collisions
	cd.update();

	DBGEND("cd");
	DBGSTART("logic");

	// Apply game logic rules
	std::vector<lif::Entity*> to_be_spawned;
#ifndef RELEASE
	int i = 0;
#endif
	for (const auto& logic : logicFunctions) {
#ifndef RELEASE
		std::stringstream n;
		n << "logic_" << i;
		dbgStats.timer.start(n.str());
#endif
		entities.apply(logic, *this, to_be_spawned);
#ifndef RELEASE
		dbgStats.timer.end(n.str());
		++i;
#endif
	}

	DBGEND("logic");

	std::optional<sf::Vector2i> lastBombPlaced;
	for (auto e : to_be_spawned) {
		_spawn(e);
		if (dynamic_cast<lif::Bomb*>(e))
			lastBombPlaced = lif::tile(e->getPosition()) - sf::Vector2i { 1, 1 };
	}

	if (lastBombPlaced)
		bombFormations.addBomb(*this, *lastBombPlaced);

	bombFormations.debugDisplay();

	DBGSTART("ent_update");

	// Update entities and their components
	entities.updateAll();

	DBGEND("ent_update");
	DBGEND("tot");
}

void BaseLevelManager::_spawn(lif::Entity *e) {
	entities.add(e);
}

void BaseLevelManager::reset() {
	entities.clear();
}

void BaseLevelManager::pause() {
	lif::time.pause();
	paused = true;
}

void BaseLevelManager::resume() {
	lif::time.resume();
	paused = false;
}

void BaseLevelManager::disableInputFor(const sf::Time& time) {
	inputDisableTime = time;
	inputDisableClock.restart();
}

bool BaseLevelManager::isInputDisabled() const {
	return inputDisableClock.getElapsedTime() < inputDisableTime;
}
