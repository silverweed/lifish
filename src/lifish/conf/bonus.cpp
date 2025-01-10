#include "bonus.hpp"

/** Relative bonus probabilities (order is important) */
std::discrete_distribution<unsigned> lif::conf::bonus::distribution {
	15,	// max bombs
	15,	// quick fuse
	15,	// max range
	15,	// shield
	15,	// speedy
	3,	// zapper
	3,	// sudden death
	15,	// health small
	15,	// health full
	889	// no bonus
};
