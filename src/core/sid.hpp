#pragma once

#include <cstdint>
#include <string>
#ifndef RELEASE
	#include <unordered_map>
#endif
#include "utils.hpp"

namespace lif {

namespace hashing {

#ifdef _WIN32
inline uint32_t fnv1_hash(const char* buffer) {
#else
constexpr uint32_t fnv1_hash(const char* buffer) {
#endif
	constexpr uint32_t fnv_prime32 = 16777619;
	uint32_t result = 2166136261;
	int i = 0;
	while (buffer[i] != '\0') {
		result *= fnv_prime32;
		result ^= static_cast<uint32_t>(buffer[i++]);
	}
	return result;
}

}

using StringId = uint32_t;

#ifndef RELEASE

class StringIdMap : public std::unordered_map<StringId, std::string> {
public:
	void addUnique(StringId key, const std::string& value) {
		auto it = find(key);
		if (it == end()) {
			emplace(key, value);
			return;
		}

		// If key is already in the map, the value must be the same
		if (it->second != value) {
			throw std::invalid_argument("Two strings match the same StringId: '" + it->second
				+ "'" + " and '" + value + "' !!!");
		}
	}
};

// Maps StringId => original string
extern StringIdMap hashedStringDb;

#endif

#ifdef RELEASE

#ifdef _WIN32
inline StringId sid(const char *buf) {
#else
constexpr StringId sid(const char *buf) {
#endif
	return hashing::fnv1_hash(buf);
}

inline StringId sid(const std::string& str) {
	return hashing::fnv1_hash(str.c_str());
}

inline std::string sidToString(StringId id) {
	return lif::to_string(static_cast<uint32_t>(id));
}

#else

inline StringId sid(const char *buf) {
	const auto h = hashing::fnv1_hash(buf);
	hashedStringDb.addUnique(h, std::string{ buf });
	return h;
}

inline StringId sid(const std::string& str) {
	const auto h = hashing::fnv1_hash(str.c_str());
	hashedStringDb.addUnique(h, str);
	return h;
}

inline std::string sidToString(StringId id) {
	return lif::hashedStringDb[id];
}
#endif

} // end namespace lif

