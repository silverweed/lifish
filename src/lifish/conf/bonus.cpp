#include "bonus.hpp"

/** Relative bonus probabilities */
std::discrete_distribution<unsigned> lif::conf::bonus::distribution {
	10,	// max bombs
	10,	// max range
	1,	// absorb
	1,	// armor
	7,	// quick fuse
	3,	// incendiary bomb
	1,	// throwable bomb
	10,	// shield
	10,	// speedy
	3,	// zapper
	2,	// sudden death
	10,	// health small
	7,	// health full
	808	// no bonus
};
