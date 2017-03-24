#include "bonus.hpp"

/** Relative bonus probabilities */
std::discrete_distribution<unsigned> lif::conf::bonus::distribution {
	10,	// max bombs
	7,	// quick fuse
	10,	// max range
	10,	// shield
	10,	// speedy
	4,	// incendiary bomb
	1,	// throwable bomb
	2,	// zapper
	2,	// sudden death
	10,	// health small
	7,	// health full
	803	// no bonus
};
