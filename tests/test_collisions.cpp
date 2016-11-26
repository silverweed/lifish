#include "game.hpp"
#include "SHCollisionDetector.hpp"
#include "LevelManager.hpp"
#include "Temporary.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "GameContext.hpp"
#include "Entity.hpp"
#include "GameContext.hpp"
#include "LevelSet.hpp"
#include "FreeMoving.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>

#define N_RAND 100001

using namespace Game;
using std::cerr;
using std::endl;

float randomNumbers[N_RAND];
unsigned curRand = 0;

float nextRand() {
	const float r = randomNumbers[curRand++];
	if (curRand == N_RAND) curRand = 0;
	return r;
}

class Square : public Entity {
	FreeMoving *moving = nullptr;
	Collider *collider = nullptr;

	int nstuck = 0;
	Game::Layers::Layer mlayer = Layers::DEFAULT;

	void _changeDirection() {
		moving->setVelocity(sf::Vector2f(nextRand(), nextRand()));	
	}
public:
	Square(const sf::Vector2f& pos) : Entity(pos) {
		moving = addComponent(new FreeMoving(*this, 100.0));
		addComponent(new Drawable(*this, *addComponent(new Sprite(*this, Game::getAsset("test", "square.png"), sf::IntRect(0, 0, 10, 10)))));
		collider = addComponent(new Collider(*this, [this] (Collider& c) {
			if (c.getLayer() == mlayer)
				_changeDirection();
		}, mlayer, sf::Vector2i(10, 10)));
		addComponent(new Temporary(*this, [this] () { return nstuck > 4; }));
	}

	Entity* init() override {
		Entity::init();
		_changeDirection();
		return this;
	}

	void update() override {
		Entity::update();
		if (collider->isAtLimit()) {
			nstuck++;
			moving->setVelocity(-moving->getVelocity());
			position = position + sf::Vector2f(moving->getVelocity().x*2,moving->getVelocity().y*2);;
		} else
			nstuck = 0;
	}
};

void print_cd_stats(const LevelManager&);
void fill_random();

int main() {
	Game::init();
	std::cerr << "pwd = " << Game::pwd << std::endl;

	for (int i = 0; i < N_RAND; ++i)
		randomNumbers[i] = float(rand())/RAND_MAX;

	Game::options.windowSize = sf::Vector2u(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	sf::RenderWindow window(
			sf::VideoMode(Game::options.windowSize.x, Game::options.windowSize.y),
			"test collisions");
	
	MusicManager mm;
	Game::musicManager = &mm;

	GameContext game(window, std::string(Game::pwd) + Game::DIRSEP + ".." + Game::DIRSEP + std::string("levels.json"), 1);
	sf::Vector2f origin(-Game::SIDE_PANEL_WIDTH, 0);
	game.setOrigin(origin);
	LevelManager& lm = game.getLM();

	for (int i = 0; i < 300; ++i) {
		float x = 2 * TILE_SIZE + i * 12 + 1;
		float y = 2 * TILE_SIZE + 1;
		while (x >= LEVEL_WIDTH * TILE_SIZE) {
			x -= LEVEL_WIDTH * TILE_SIZE;
			y += 12;
		}
		std::cerr<<"x = " <<x << ", y = " << y << endl;
		lm.getEntities().add(new Square(sf::Vector2f(x, y)));
	}

	options.showFPS = true;

	while (!std::cin.eof()) {
		game.handleEvents(window);

		game.update();
		print_cd_stats(lm);

		window.clear();
		window.draw(game);
		maybeShowFPS(window);
		window.display();
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
