#pragma once

#include "Direction.hpp"
#include <array>
#include <string>
#include <unordered_map>

namespace lif {

namespace ui {

class TransitionGraph {
	using NodeTransitions = std::array<std::string, lif::Direction::NONE>;

	/** The first node inserted in this graph */
	std::string firstNode;
	std::unordered_map<std::string, NodeTransitions> transitions;

public:
	explicit TransitionGraph() {}

	void add(const std::string& node, std::pair<lif::Direction, std::string> pair) {
		if (firstNode.length() == 0)
			firstNode = node;

		auto& tnode = transitions[node];
		if (tnode[pair.first].length() != 0)
			throw std::logic_error("add(" + node + ", " + lif::directionToString(pair.first)
					+ ") called twice!");
		tnode[pair.first] = pair.second;

		// If there is no opposite transition yet, create it
		auto& tsec = transitions[pair.second];
		const auto opp = lif::oppositeDirection(pair.first);
		if (tsec[opp].length() == 0)
			tsec[opp] = node;
	}

	std::string get(const std::string& node, lif::Direction d) const {
		if (d < 0 || d >= lif::Direction::NONE) return "";
		const auto tr = transitions.find(node);
		if (tr == transitions.end()) return "";
		return tr->second[d];
	}

	std::string first() const {
		return firstNode;
	}

	void clear() {
		transitions.clear();
		firstNode = "";
	}

	size_t size() const {
		return transitions.size();
	}
};

}

}
