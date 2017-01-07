#include "bonus.hpp"

/** Relative bonus probabilities */
std::discrete_distribution<unsigned short> lif::conf::bonus::distribution {
	5,   // max bombs
	4,   // quick fuse
	5,   // max range
	5,   // shield
	5,   // speedy
	2,   // incendiary bomb
	1,   // zapper
	1,   // sudden death
	5,   // health small
	4,   // health full
	518  // no bonus
};
