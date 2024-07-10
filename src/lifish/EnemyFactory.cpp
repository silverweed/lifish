#include "EnemyFactory.hpp"
#include "AI.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "Sighted.hpp"
#include "collision_layers.hpp"

using lif::EnemyFactory;

std::unique_ptr<lif::Enemy> EnemyFactory::create(const lif::LevelManager& lm, unsigned id, const sf::Vector2f& pos) {

	const auto& info = lm.getLevel()->getLevelSet().getEnemyInfo(id);

	auto enemy = std::make_unique<lif::Enemy>(pos, id, info);
	enemy->get<lif::AI>()->setLevelManager(&lm);
	auto sighted = enemy->get<lif::Sighted>();
	sighted->setEntityGroup(&lm.getEntities());
	sighted->setOpaque({ lif::c_layers::BREAKABLES, lif::c_layers::UNBREAKABLES });

	return enemy;
}
