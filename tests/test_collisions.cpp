#include "game.hpp"
#include "SHCollisionDetector.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include <iostream>
#include <iomanip>

using namespace Game;

void print_cd_stats(const LevelManager&);

int main() {
	Game::init();
	std::cerr << "pwd = " << Game::pwd << std::endl;

	LevelManager lm;
	LevelSet ls(std::string(Game::pwd) + Game::DIRSEP + ".." + Game::DIRSEP + std::string("levels.json"));

	lm.setLevel(ls, 1);

	while (!std::cin.eof()) {
		lm.update();
		print_cd_stats(lm);
	}
}

void print_cd_stats(const LevelManager& lm) {
	const auto& dbgStats = lm.getCollisionDetector().getStats();
	std::cerr << std::setfill(' ') << std::scientific << std::setprecision(4)
		<< "#checked: " << std::setw(5) << dbgStats.counter.safeGet("checked")
		<< " | tot: " << std::setw(8) << dbgStats.timer.safeGet("tot")
		<< " | tot_narrow: " << std::setw(8) << dbgStats.timer.safeGet("tot_narrow")
		<< " | setup: " << std::setw(8) << dbgStats.timer.safeGet("setup") 
		<< " | average: " << std::setw(8) 
			<< dbgStats.timer.safeGet("tot_narrow")/dbgStats.counter.safeGet("checked")
		<< std::resetiosflags(std::ios::showbase) << std::endl;
}
