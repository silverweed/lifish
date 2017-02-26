#pragma once

#include "TimeStats.hpp"
#include "Counter.hpp"

#ifndef RELEASE
#	define DBGSTART(name) \
		dbgStats.timer.start(name)
#	define DBGEND(name) \
		dbgStats.timer.end(name)
#else
#	define DBGSTART(name)
#	define DBGEND(name)
#endif

namespace lif {

namespace debug {

struct Stats {
	TimeStats timer;
	Counter counter;
};

}

}
