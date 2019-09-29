#pragma once

#include <vector>
#include <SFML/System/NonCopyable.hpp>

namespace lif {

/**
 * GlobalDataPipe is a singleton class handling arbitrary cross-hierarchy data transfers in a
 * type-safe fashion.
 * Use of this class should be a last-resort when data needs to be passed "upwards" to managing
 * objects whose access is not available directly.
 */
template<class T>
class GlobalDataPipe final : private sf::NonCopyable {

	std::vector<T> pending;

	explicit GlobalDataPipe() {}
public:
	static GlobalDataPipe<T>& getInstance() {
		static GlobalDataPipe<T> instance;
		return instance;
	}

	template<class... Args>
	void add(Args&&... args) { pending.emplace_back(args...); }
	void add(T data) { pending.emplace_back(data); }
	void add(T&& data) { pending.emplace_back(data); }
	bool hasData() const { return pending.size() > 0; }
	T pop() { T back = pending.back(); pending.pop_back(); return back; }
	void clear() { pending.clear(); }
};

}
