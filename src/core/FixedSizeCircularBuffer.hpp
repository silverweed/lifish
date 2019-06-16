#pragma once

#include <array>
#include <cstddef>

namespace lif {

template<typename T, int N>
class FixedSizeCircularBuffer {
	std::array<T, N> buf;
	std::size_t head = 0;
	std::size_t tail = 0;
	bool full = false;

public:
	explicit FixedSizeCircularBuffer() {}

	void reset() {
		full = false;
		head = tail;
	}

	bool isEmpty() const {
		return !full && head == tail;
	}

	constexpr std::size_t capacity() const { return N; }

	std::size_t size() const {
		const int diff = head - tail;
		const int h = diff >= 0;
		return full * N + (1 - full) * (diff + N * (1 - h));
	}

	bool push(const T& item) {
		bool overwritten = false;

		buf[head] = item;

		if (full) {
			tail = (tail + 1) % N;
			overwritten = true;
		}

		head = (head + 1) % N;

		full = head == tail;

		return overwritten;
	}

	T pop() {
		if (isEmpty())
			throw std::out_of_range("pop from an empty CircularBuffer!");

		T val;
		std::swap(val, buf[tail]);
		full = false;
		tail = (tail + 1) % N;

		return val;
	}

	class iterator {

		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::size_t;
		using pointer = T*;
		using reference = T&;

		lif::FixedSizeCircularBuffer<T, N>& cbuf;
		std::size_t at = 0;
	public:
		explicit iterator(lif::FixedSizeCircularBuffer<T, N>& cbuf, std::size_t at)
			: cbuf(cbuf)
			, at(at)
		{}

		iterator& operator++() {
			++at;
			return *this;
		}

		iterator operator++(int) {
			iterator ret = *this;
			++(*this);
			return ret;
		}

		bool operator==(iterator other) const {
			return &cbuf == &other.cbuf && at == other.at;
		}

		bool operator!=(iterator other) const {
			return !(*this == other);
		}

		reference operator*() { return cbuf.buf[(cbuf.tail + at) % N]; }
	};

	class const_iterator {

		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::size_t;
		using pointer = const T*;
		using reference = const T&;

		const lif::FixedSizeCircularBuffer<T, N>& cbuf;
		std::size_t at = 0;
	public:
		explicit const_iterator(const lif::FixedSizeCircularBuffer<T, N>& cbuf, std::size_t at)
			: cbuf(cbuf)
			, at(at)
		{}

		const_iterator& operator++() {
			++at;
			return *this;
		}

		const_iterator operator++(int) {
			const_iterator ret = *this;
			++(*this);
			return ret;
		}

		bool operator==(const_iterator other) const {
			return &cbuf == &other.cbuf && at == other.at;
		}

		bool operator!=(const_iterator other) const {
			return !(*this == other);
		}

		reference operator*() const { return cbuf.buf[(cbuf.tail + at) % N]; }
	};

	iterator begin() { return iterator(*this, 0); }
	iterator end() { return iterator(*this, size()); }

	const_iterator begin() const { return const_iterator(*this, 0); }
	const_iterator end() const { return const_iterator(*this, size()); }
};

}
