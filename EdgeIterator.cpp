#include "pch.h"
#include "Graph.h"

namespace versioned_graph {
	/*
	 * edge iterator
	 */

	graph::edge_iterator& graph::edge_iterator::operator++() {
		it = next(it, ver);
		return *this;
	}
	graph::edge_iterator& graph::edge_iterator::operator--() {
		it = prev(it, ver);
		return *this;
	}

	graph::edge_iterator graph::edge_iterator::operator+(size_t delta) {
		edge_iterator eit = *this;
		for (; delta > 0; --delta)
			++eit;
		return eit;
	}
	const graph::edge_iterator graph::edge_iterator::operator+(size_t delta) const {
		edge_iterator eit = *this;
		for (; delta > 0; --delta)
			++eit;
		return eit;
	}
	graph::edge_iterator graph::edge_iterator::operator-(size_t delta) {
		edge_iterator eit = *this;
		for (; delta > 0; --delta)
			--eit;
		return eit;
	}
	const graph::edge_iterator graph::edge_iterator::operator-(size_t delta) const {
		edge_iterator eit = *this;
		for (; delta > 0; --delta)
			--eit;
		return eit;
	}

	void graph::edge_iterator::setNext(_iterator target) {
		it->next = target;
		it->next_v = ver;
	}
	void graph::edge_iterator::setPrev(_iterator target) {
		it->prev = target;
		it->prev_v = ver;
	}

	bool graph::edge_iterator::operator==(const edge_iterator& other) const {
		return it == other.it && ver == other.ver;
	}
	bool graph::edge_iterator::operator!=(const edge_iterator& other) const {
		return it != other.it || ver != other.ver;
	}
	vertex graph::edge_iterator::get_source() const {
		if (it->reversed_v == ver)
			return it->t;
		else
			return it->s;
		//return it->get_source(ver); // A lot slower. Why?
	}
	vertex graph::edge_iterator::get_target() const {
		if (it->reversed_v == ver)
			return it->s;
		else
			return it->t;
		//return it->get_target(ver); // A lot slower. Why?
	}

#ifdef ENABLE_MARKING
	bool graph::edge_iterator::is_marked() const {
		return it->is_marked(mark_v);
	}
	void graph::edge_iterator::mark() {
		it->mark(mark_v);
	}
#endif

	bool graph::edge_iterator::is_edge() const {
		return it->t != it->s;
	}
}