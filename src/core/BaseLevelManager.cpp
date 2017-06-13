#include "BaseLevelManager.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"

using lif::BaseLevelManager;

BaseLevelManager::BaseLevelManager()
	: cd(entities)
{}

void BaseLevelManager::update() {
	DBGSTART("tot");
	DBGSTART("reset_align");

	// Set prevAligns for aligned entities
	entities.apply([] (lif::Entity *e) {
		if (!e->isAligned()) return;
		auto moving = e->get<lif::AxisMoving>();
		if (moving == nullptr) return;
		moving->setPrevAlign(lif::tile(e->getPosition()));
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
	for (auto logic : logicFunctions) {
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

	for (auto e : to_be_spawned)
		_spawn(e);

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
	entities.apply([] (lif::Entity *e) {
		auto clocks = e->getAllRecursive<lif::Clock>();
		for (auto clock : clocks)
			clock->pause();
	});
	paused = true;
}

void BaseLevelManager::resume() {
	entities.apply([] (lif::Entity *e) {
		auto clocks = e->getAllRecursive<lif::Clock>();
		for (auto clock : clocks)
			clock->resume();
	});
	paused = false;
}

void BaseLevelManager::tickClocks(const sf::Time& delta) {
	entities.apply([delta] (lif::Entity *e) {
		auto clocks = e->getAllRecursive<lif::Clock>();
		for (auto clock : clocks)
			clock->add(delta);
	});
}

void BaseLevelManager::disableInputFor(const sf::Time& time) {
	inputDisableTime = time;
	inputDisableClock.restart();
}

bool BaseLevelManager::isInputDisabled() const {
	return inputDisableClock.getElapsedTime() < inputDisableTime;
}
