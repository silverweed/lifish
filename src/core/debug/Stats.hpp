#pragma once

#include "TimeStats.hpp"
#include "Counter.hpp"

namespace lif {

namespace debug {

struct Stats {
	TimeStats timer;
	Counter counter;
};

}

}
