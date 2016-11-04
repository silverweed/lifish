#pragma once

#include "TimeStats.hpp"
#include "Counter.hpp"

namespace Game {

namespace Debug {

struct Stats {
	TimeStats timer;
	Counter counter;
};

}

}
