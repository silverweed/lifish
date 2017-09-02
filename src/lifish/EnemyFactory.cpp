#include "EnemyFactory.hpp"
#include "Wisp.hpp"
#include "AlienPredator.hpp"
#include "AI.hpp"
#include "collision_layers.hpp"
#include "LevelManager.hpp"
#include "Level.hpp"
#include "Sighted.hpp"
#include "LevelSet.hpp"
#include "LeapingMovement.hpp"

using lif::EnemyFactory;

std::unique_ptr<lif::Enemy> EnemyFactory::create(lif::LevelManager& lm, unsigned id, const sf::Vector2f& pos) {

	std::unique_ptr<lif::Enemy> enemy;
	const auto& info = lm.getLevel()->getLevelSet().getEnemyInfo(id);

	// Some enemies have their own classes, others are just 'Enemy'
	switch (id) {
	case 5:
		enemy = std::make_unique<lif::Wisp>(pos, info);
		break;
	case 10:
		enemy = std::make_unique<lif::AlienPredator>(pos, info);
		break;
	default:
		enemy = std::make_unique<lif::Enemy>(pos, id, info);
		break;
	}
	enemy->get<lif::AI>()->setLevelManager(&lm);
	auto sighted = enemy->get<lif::Sighted>();
	sighted->setEntityGroup(&lm.getEntities());
	sighted->setOpaque({ lif::c_layers::BREAKABLES, lif::c_layers::UNBREAKABLES });

	// FIXME
	if (id == 4)
		enemy->addComponent<lif::LeapingMovement>(*enemy, sf::seconds(0.2))->init();

	return enemy;
}
