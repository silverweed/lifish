#include "GodEyeBoss.hpp"
#include "Lifed.hpp"
#include "collision_utils.hpp"
#include "LevelManager.hpp"
#include "Animated.hpp"
#include "Controllable.hpp"
#include "Torch.hpp"
#include "Spikes.hpp"
#include "EnemyFactory.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "Collider.hpp"
#include "Level.hpp"
#include "HurtDrawProxy.hpp"
#include "Player.hpp"
#include "FreeSighted.hpp"
#include "Clock.hpp"
#include "LightSource.hpp"
#include "Sounded.hpp"
#include "Drawable.hpp"
#include "Enemy.hpp"
#include "Bonusable.hpp"
#include "BufferedSpawner.hpp"
#include "Flare.hpp"
#include "Fixed.hpp"
#include "game.hpp"
#include "conf/boss.hpp"
#include "camera_utils.hpp"
#include "entity_type.hpp"
#include "spawn_functions.hpp"
#include <cmath>
#include <random>
#include <array>
#include <string>
#include <algorithm>
#include <vector>

using lif::GodEyeBoss;
using lif::TILE_SIZE;
using namespace lif::conf::boss::god_eye_boss;

const static sf::Vector2f SIZE(3 * TILE_SIZE, 3 * TILE_SIZE);
const static sf::Vector2f SHIELD_SIZE(4 * TILE_SIZE, 4 * TILE_SIZE);

const static std::array<std::string, 4> LEVEL_CONFIGURATIONS = {{
	// BOSS_RIGHT
	"X00100t100000000101t12121021210000010000022002101010111010000120^0t100^20000111010101120000001000"
	"^0002000110101110102000002000000002000210111110101000t00001^00102200011121010112121Y0000001000000t",
	// BOSS_LEFT
	"t0000001000000Y12121101012111000220100^10000t0001010111110120002000000002000002010111010110002000"
	"^00010000002110101011100002^001t0^02100001011101010120022000001000001212012121t101000000001t00100X",
	// BOSS_TOP
	"00100000000010Yt0200000000020001221010101221000^00202020000011011101t10111000200200010000^0111010"
	"12101t110201010000^2010010101t1011001001000200200102001010111^1010100020100000200000010201t101010X",
	// BOSS_BOTTOM
	"X010101t1020100000020000010200010101^1110101002010020020001001001101t1010100102^000010102011t1012"
	"10101110^00001000200200011101t10111011000002020200^0001221010101221000200000000020tY01000000000100",
}};

GodEyeBoss::GodEyeBoss(const sf::Vector2f& pos, lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
{
	addComponent<lif::Fixed>(*this);
	spriteBg = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "god_eye_boss.png"));
	spriteBg->addAnimation("idle", {
		sf::IntRect(0 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(0 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
	}, true);
	spriteBg->getSprite().setFrameTime(sf::milliseconds(140));
	spriteFg = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "god_eye_boss_fg.png"));
	spriteFg->addAnimation("idle", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	shield = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "god_eye_shield.png"));
	shield->addAnimation("shield", {
		sf::IntRect(0 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(1 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(2 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(3 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(4 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
		sf::IntRect(5 * SHIELD_SIZE.x, 0, SHIELD_SIZE.x, SHIELD_SIZE.y),
	}, true);
	shield->getSprite().setFrameTime(sf::milliseconds(110));
	shield->getSprite().setColor(sf::Color(255, 255, 255, 100));
	shield->setPosition(position + sf::Vector2f{ TILE_SIZE * 3/2, TILE_SIZE * 3/2 });
	shield->getSprite().setOrigin(SHIELD_SIZE * 0.5f);
	shieldAlphaClock = addComponent<lif::Clock>(*this);

	hurtDrawProxy = addComponent<lif::HurtDrawProxy>(*this, spriteBg);
	addComponent<lif::Lifed>(*this, LIFE, [this] (int, int) {
		// on hurt
		if (++timesHurt >= TIMES_TO_HURT_BEFORE_DEATH)
			get<lif::Killable>()->kill();
		else
			_onHurt();
	});
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "god_eye_death.ogg"),
		lif::sid("hurt"), lif::getAsset("sounds", "god_eye_hurt.ogg")
	);

	_addDefaultCollider(SIZE);
	// Boss is only vulnerable when shield is down
	vulnerable = false;

	attackClock = addComponent<lif::Clock>(*this);
	sighted = addComponent<lif::FreeSighted>(*this);
	addComponent<lif::BufferedSpawner>(*this);
	//addComponent<lif::LightSource>(*this, 2 * TILE_SIZE, sf::Color(128, 128, 128))
			//->setPosition(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	addComponent<lif::Drawable>(*this, *this);
	addComponent<lif::Bonusable>(*this);
}

void GodEyeBoss::update() {
	lif::Boss::update();

	_updatePupilPos();
	_updateShield();
}

void GodEyeBoss::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*hurtDrawProxy, states);
	target.draw(*spriteFg, states);
	target.draw(*shield, states);
}

void GodEyeBoss::_updatePupilPos() {
	sf::Vector2f newpos(position.x + SIZE.x / 3.f, position.y + SIZE.y / 3.f - 3);
	const auto player = sighted->nearest<lif::Player>();
	if (player != nullptr) {
		newpos += static_cast<float>(TILE_SIZE * 0.2) * lif::normalized(player->getPosition() - newpos);
	}
	spriteFg->setPosition(newpos);
}

void GodEyeBoss::_teleportTo(const sf::Vector2f& newpos) {
	position = newpos;
	_updatePupilPos();
	shield->setPosition(position + sf::Vector2f(TILE_SIZE * 3/2, TILE_SIZE * 3/2));
}

void GodEyeBoss::_updateShield() {
	// Boss becomes vulnerable as soon as there are no enemies in the level
	vulnerable = sighted->nearest<lif::Enemy>() == nullptr;
	if (!vulnerable) {
		shield->getSprite().setColor(sf::Color(255, 255, 255,
				100 + 30 * std::sin(5 * shieldAlphaClock->getElapsedTime().asSeconds())));
		const auto sc = 1 + 0.1 * std::sin(8 * shieldAlphaClock->getElapsedTime().asSeconds());
		shield->getSprite().setScale(sc, sc);
	} else {
		shield->getSprite().setColor(sf::Color(255, 255, 255, 0));
	}
}

void GodEyeBoss::_shakeWalls() {

	lif::requestCameraShake(0.07, 125, 0.05, 125, SHAKE_DURATION, 2.5);

	// Freeze players
	for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if (player == nullptr)
			continue;
		player->setPosition(lif::aligned2(player->getPosition()));
		player->get<lif::Moving>()->stop();
		player->get<lif::Controllable>()->disableFor(SHAKE_DURATION);
	}

	std::vector<lif::BreakableWall*> breakables;
	std::vector<lif::FixedWall*> fixed;
	std::vector<lif::Torch*> torches;
	std::vector<lif::Spikes*> spikes;
	const auto maxSize = lm.getEntities().size();
	breakables.reserve(maxSize);
	fixed.reserve(maxSize);
	torches.reserve(maxSize);
	spikes.reserve(maxSize);

	const auto lvWidth = lm.getLevel()->getInfo().width;
	const auto lvHeight = lm.getLevel()->getInfo().height;

	// Collect all entities to move
	lm.getEntities().apply([lvWidth, lvHeight, &breakables, &fixed, &torches, &spikes] (lif::Entity *e) {
		if (auto w = dynamic_cast<lif::FixedWall*>(e)) {
			fixed.emplace_back(w);
			e->setPosition(sf::Vector2f(0, 0));
			return;
		}
		if (auto w = dynamic_cast<lif::BreakableWall*>(e)) {
			breakables.emplace_back(w);
			e->setPosition(sf::Vector2f(0, 0));
			return;
		}
		if (auto w = dynamic_cast<lif::Torch*>(e)) {
			const auto pos = e->getPosition();
			if (pos.x * pos.y <= 0 || pos.x > lvWidth * TILE_SIZE || pos.y > lvHeight * TILE_SIZE)
				return;
			torches.emplace_back(w);
			e->setPosition(sf::Vector2f(0, 0));
			return;
		}
		if (auto w = dynamic_cast<lif::Spikes*>(e)) {
			spikes.emplace_back(w);
			e->setPosition(sf::Vector2f(0, 0));
			return;
		}
	});

	// Take all entities to move from the current configuration and redistribute
	// them according to the new one. No wall is created or destroyed in the process, as
	// every configuration has the same number of objects.
	// If an object would be moved onto a player, it is not placed.
	std::vector<int> possibleConfigs = { 0, 1, 2, 3 };
	possibleConfigs.erase(std::remove(possibleConfigs.begin(), possibleConfigs.end(), lvConfiguration));
	static std::uniform_int_distribution<> newLevelConfigDist(0, 2);
	lvConfiguration = static_cast<LevelConfiguration>(possibleConfigs[newLevelConfigDist(lif::rng)]);

	assert(0 <= lvConfiguration && "Invalid new configuration!");

	const auto& newConf = LEVEL_CONFIGURATIONS[static_cast<int>(lvConfiguration)];

	// (UGLY): this keeps references to the torches so we can adjust them
	// after having popped them from the original vector.
	std::vector<lif::Torch*> torches2(torches);

#define MOVE_AND_POP(what, canOverlapPlayer) \
	{ \
		if (what .size() > 0) { \
			auto entity = what .back(); \
			if (canOverlapPlayer || !isPlayerAt(curPos)) { \
				entity->setPosition(curPos); \
				what .pop_back(); \
			} \
		} \
		break; \
	}

	const auto isPlayerAt = [this] (const auto& pos) {
		for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
			const auto player = lm.getPlayer(i + 1);
			if (player != nullptr && player->getPosition() == pos)
				return true;
		}
		return false;
	};

	for (unsigned curTile = 0; curTile < newConf.length(); ++curTile) {
		const sf::Vector2f curPos(TILE_SIZE * (curTile % lvWidth + 1), TILE_SIZE * (curTile / lvWidth + 1));

		switch (lif::entityFromLetter(newConf[curTile])) {
			using ET = lif::EntityType;
		case ET::FIXED:
			MOVE_AND_POP(fixed, false)
		case ET::BREAKABLE:
			MOVE_AND_POP(breakables, false)
		case ET::TORCH:
			MOVE_AND_POP(torches, true)
		case ET::SPIKES:
			MOVE_AND_POP(spikes, false)
		default:
			break;
		}
	}
#undef MOVE_AND_POP

	// Remove more and more torches as the fight proceeds
	std::random_shuffle(torches2.begin(), torches2.end());
	auto torchesToRemove = timesHurt;
	for (auto torch : torches2) {
		if (torchesToRemove > 0) {
			auto light = torch->get<lif::LightSource>();
			if (light->isActive()) {
				light->setActive(false);
				torch->get<lif::Animated>()->setAnimation("off");
				--torchesToRemove;
			}
		}
		torch->fixOrientation(newConf, lvWidth);
	}
}

void GodEyeBoss::_onHurt() {
	// Shake walls, teleport away and spawn enemies

	// Become invulnerable right away to prevent more explosion to damage the boss
	vulnerable = false;

	/// Shake walls. We need to do this first to know where to teleport.
	_shakeWalls();

	/// Teleport
	const std::vector<sf::Vector2f> teleportPositions({
		sf::Vector2f(13, 6) * static_cast<float>(TILE_SIZE), // boss right
		sf::Vector2f(1, 6)  * static_cast<float>(TILE_SIZE), // boss left
		sf::Vector2f(7, 1)  * static_cast<float>(TILE_SIZE), // boss top
		sf::Vector2f(7, 11) * static_cast<float>(TILE_SIZE), // boss bottom
	});
	_teleportTo(teleportPositions[static_cast<int>(lvConfiguration)]);

	/// Spawn enemies
	auto spawner = get<lif::BufferedSpawner>();
	lif::spawnInFreeTiles(spawner, lm, SPAWNED_ENEMY_ID, N_ENEMIES_SPAWNED, 4, [] (lif::Enemy *enemy) {
		enemy->get<lif::Moving>()->block(SHAKE_DURATION);
	});
}

void GodEyeBoss::_kill() {
	lif::Boss::_kill();
	get<lif::BufferedSpawner>()->addSpawned(new lif::Flare(sf::seconds(0), sf::seconds(60)));
	lif::requestCameraShake(0.08, 120, 0, 0, sf::seconds(4), 2);
}
