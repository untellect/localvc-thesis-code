#pragma once

#include "Graph.h"
#include "Graph2.h"
#include <vector>
#include <unordered_set>
#include <numeric>
#include "Random.h"

template<typename T>
T select_randomly(std::vector<T> vec) {
	return vec[random_int(0, vec.size() - 1)];
}

namespace versioned_graph {
	std::unordered_set<vertex> get_neighbors(const std::vector<std::vector<vertex>>& g, const std::unordered_set<vertex>& S);
	std::unordered_set<vertex> get_reachable(const std::vector<std::vector<vertex>>& g, vertex x);

	std::unordered_set<vertex> get_neighbors(graph& g, const std::unordered_set<vertex>& S);
	std::unordered_set<vertex> get_reachable(graph& g, vertex x);

	size_t get_reachable_volume(graph& g, vertex x);
	size_t get_vertex_cut_volume(graph& g, vertex x, std::unordered_set<vertex> cut);

	// Samples an edge and provides the source vertex
	struct edge_source_sampler {
		std::vector<vertex> data;
		size_t i = 0;

		edge_source_sampler(const graph& g);
		edge_source_sampler(const std::vector<std::vector<vertex>>& g);
		vertex get_vertex();
		size_t n_edges();
		void reset();
	};
}

namespace resettable_graph {
	std::unordered_set<vertex> get_neighbors(graph& g, const std::unordered_set<vertex>& S);
	std::unordered_set<vertex> get_reachable(graph& g, vertex x);

	size_t get_reachable_volume(graph& g, vertex x);
	size_t get_vertex_cut_volume(graph& g, vertex x, std::unordered_set<vertex> cut);
}

struct union_find {
	std::vector<size_t> data;

	union_find(size_t n) : data(n) {
		std::iota(data.begin(), data.end(), 0);
	}
	size_t find(size_t i) {
		size_t j = i;
		while (data[j] != j) j = data[j];
		while (data[i] != i) {
			size_t tmp = data[i];
			data[i] = j;
			i = tmp;
		}
		return i;
	}
	void merge(size_t i, size_t j) {
		i = find(i);
		data[i] = j;
	}
};