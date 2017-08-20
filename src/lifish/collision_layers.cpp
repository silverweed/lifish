#include "collision_layers.hpp"

/** Defines layers `a` and `b` to collide with each other.
 *  If two layers collide, the `onCollision` callback of a collider on layer `a`
 *  will be called whenever its collider intersects that of one on layer `b`.
 *  This does NOT imply the layers are solid for each other, but merely that they
 *  interact.
 */
#define COLLIDES(a, b) \
	collide[a][b] = true; \
	collide[b][a] = true;

#define DOES_NOT_COLLIDE(a, b) \
	collide[a][b] = false; \
	collide[b][a] = false; \
	solid[a][b] = false; \
	solid[b][a] = false;

/** Defines layer `a` to be "solid" for `b` and vice-versa.
 *  Implies COLLIDES(a, b).
 */
#define SOLID(a, b) \
	solid[a][b] = true; \
	solid[b][a] = true; \
	collide[a][b] = true; \
	collide[b][a] = true;

using L = lif::c_layers::Layer;

lif::Matrix<bool, L::N_LAYERS, L::N_LAYERS> lif::c_layers::collide,
	                                    lif::c_layers::solid;

void lif::c_layers::init() {
	for (auto& l : collide)
		l.fill(false);
	for (auto& l : solid)
		l.fill(false);

	/// Collisions ///

	// Default layer collides with everything
	collide[L::DEFAULT].fill(true);
	// ...except with Teleport; else, entities like Fire would prevent teleporting
	DOES_NOT_COLLIDE(L::DEFAULT, L::TELEPORTS)

	COLLIDES(L::PLAYERS, L::ENEMIES)
	COLLIDES(L::PLAYERS, L::ENEMIES_IGNORE_BREAKABLES)
	COLLIDES(L::PLAYERS, L::BOSSES)
	COLLIDES(L::PLAYERS, L::TELEPORTS)
	COLLIDES(L::PLAYERS, L::GRABBABLE)
	COLLIDES(L::PLAYERS, L::ENEMY_BULLETS)
	COLLIDES(L::PLAYERS, L::BOSS_BULLETS)

	COLLIDES(L::ENEMIES, L::TELEPORTS)
	COLLIDES(L::ENEMIES, L::ENEMY_BULLETS)
	COLLIDES(L::ENEMIES, L::BOSS_BULLETS)

	COLLIDES(L::ENEMIES_IGNORE_BREAKABLES, L::BREAKABLES)
	COLLIDES(L::ENEMIES_IGNORE_BREAKABLES, L::TELEPORTS)
	COLLIDES(L::ENEMIES_IGNORE_BREAKABLES, L::ENEMY_BULLETS)
	COLLIDES(L::ENEMIES_IGNORE_BREAKABLES, L::BOSS_BULLETS)

	COLLIDES(L::EXPLOSIONS, L::PLAYERS)
	COLLIDES(L::EXPLOSIONS, L::ENEMIES)
	COLLIDES(L::EXPLOSIONS, L::ENEMIES_IGNORE_BREAKABLES)
	COLLIDES(L::EXPLOSIONS, L::BOSSES)
	COLLIDES(L::EXPLOSIONS, L::BOMBS)

	COLLIDES(L::ENEMY_EXPLOSIONS, L::PLAYERS)
	COLLIDES(L::ENEMY_EXPLOSIONS, L::BOMBS)

	/// Solidity ///

	SOLID(L::PLAYERS, L::PLAYERS)
	SOLID(L::PLAYERS, L::BREAKABLES)
	SOLID(L::PLAYERS, L::UNBREAKABLES)
	SOLID(L::PLAYERS, L::TRANSP_WALLS)

	SOLID(L::ENEMIES, L::ENEMIES)
	SOLID(L::ENEMIES, L::ENEMIES_IGNORE_BREAKABLES)
	SOLID(L::ENEMIES, L::BOSSES)
	SOLID(L::ENEMIES, L::BREAKABLES)
	SOLID(L::ENEMIES, L::UNBREAKABLES)
	SOLID(L::ENEMIES, L::TRANSP_WALLS)

	SOLID(L::ENEMIES_IGNORE_BREAKABLES, L::ENEMIES_IGNORE_BREAKABLES)
	SOLID(L::ENEMIES_IGNORE_BREAKABLES, L::BOSSES)
	SOLID(L::ENEMIES_IGNORE_BREAKABLES, L::UNBREAKABLES)
	SOLID(L::ENEMIES_IGNORE_BREAKABLES, L::TRANSP_WALLS)

	SOLID(L::BOSSES, L::BOSSES)
	SOLID(L::BOSSES, L::ENEMY_BULLETS)
	SOLID(L::BOSSES, L::BREAKABLES)
	SOLID(L::BOSSES, L::UNBREAKABLES)

	SOLID(L::ENEMY_BULLETS, L::BREAKABLES)
	SOLID(L::ENEMY_BULLETS, L::UNBREAKABLES)

	// "Solid" blocks for Explosions block the propagation
	SOLID(L::EXPLOSIONS, L::BREAKABLES)
	SOLID(L::EXPLOSIONS, L::UNBREAKABLES)
	SOLID(L::EXPLOSIONS, L::TRANSP_WALLS)

	SOLID(L::ENEMY_EXPLOSIONS, L::BREAKABLES)
	SOLID(L::ENEMY_EXPLOSIONS, L::UNBREAKABLES)
	SOLID(L::ENEMY_EXPLOSIONS, L::TRANSP_WALLS)

	SOLID(L::BOMBS, L::BREAKABLES)
	SOLID(L::BOMBS, L::UNBREAKABLES)
	SOLID(L::BOMBS, L::BOMBS)
}
