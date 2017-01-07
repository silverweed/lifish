#pragma once

#include "TimeStats.hpp"
#include "Counter.hpp"

namespace lif {

namespace Debug {

struct Stats {
	TimeStats timer;
	Counter counter;
};

}

}
