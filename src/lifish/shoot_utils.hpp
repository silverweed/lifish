#pragma once

namespace lif {

class Entity;

/**
 * @param shooter An Entity with:
 *	1. a FreeSighted
 *	2. an AutoShooting
 * 	(e.g. a ShootingPoint).
 * Action: shoots a FreeBullet with direction going from `shooter` to the nearest player in sight.
 */
void shootToNearestPlayer(lif::Entity& shooter);

}
