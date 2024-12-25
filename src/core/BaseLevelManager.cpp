#include "BaseLevelManager.hpp"
#include "AxisMoving.hpp"
#include "Clock.hpp"
#include "Time.hpp"
#include "Bomb.hpp"
#include "game.hpp"
#include <optional>
#include "DebugPainter.hpp"
#include "bomb_formations.hpp"

using lif::BaseLevelManager;

namespace {
constexpr auto LEVEL_WIDTH = lif::GAME_WIDTH / lif::TILE_SIZE - 2;
constexpr auto LEVEL_HEIGHT = lif::GAME_HEIGHT / lif::TILE_SIZE - 2;

using Grid = char[LEVEL_HEIGHT * LEVEL_WIDTH];

using BombPattern = std::vector<const char*>;

const BombPattern bombPatterns[] = {
	{"* *"},
	{"* *", 
	 "  *"},
	{"* * * *",
   "  * *  "},
};

void checkBombCombos(lif::BaseLevelManager& blm) {
	Grid grid;
	std::cout << LEVEL_HEIGHT << " x " << LEVEL_WIDTH << "\n";
	dumpBombsToGrid(blm, grid);

	// for (int row = 0; row < LEVEL_HEIGHT; ++row) {
	// 	for (int col = 0; col < LEVEL_WIDTH; ++col) {
	// 		std::cout << grid[row * LEVEL_WIDTH + col] << " ";
	// 	}
	// 	std::cout << "\n";
	// }
}
}

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

	static std::vector<BombFormation> formations;
	if (lastBombPlaced)
		if (auto form = buildBombFormation(*this, *lastBombPlaced))
			formations.push_back(*form);

	int formIdx = 0;
	for (const auto& formation : formations) {
		static const sf::Color colors[] = { sf::Color::Red, sf::Color::Yellow, sf::Color::Blue, sf::Color::Green };
		for (auto tile : formation)
				lif::debugPainter->addCircleAt(sf::Vector2f(tile + sf::Vector2i { 1, 1 }) * float(lif::TILE_SIZE), lif::TILE_SIZE * 0.5, 
				                               colors[formIdx % std::size(colors)]);
		++formIdx;
	}

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
