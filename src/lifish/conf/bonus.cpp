#include "bonus.hpp"

/** Relative bonus probabilities (order is important) */
std::discrete_distribution<unsigned> lif::conf::bonus::distribution {
	20,	// max bombs
	20,	// max range
	14,	// quick fuse
	20,	// shield
	20,	// speedy
	3,	// incendiary bomb
	1,	// throwable bomb
	1,	// absorb
	1,	// armor
	6,	// zapper
	4,	// sudden death
	20,	// health small
	14,	// health full
	3000	// no bonus
};
