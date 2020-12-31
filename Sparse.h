#pragma once

#include "Graph.h"

#include <vector>

namespace sparse {
	using namespace versioned_graph;

	struct edge {
		vertex x;
		vertex y;
		size_t k;
	}; // The edge (x, y) is part of the forest F_k

	struct labelling {
		std::vector<edge> edges;
		vertex lastVertex; // which has degree exactly k in FG_k
	};

	// Takes an bidirectional (both direcctions included) adjacency list for a graph.
	// Outputs a sparse list labelling in the format of an edge list (only one direction included) sorted by k, then x.
	graph nagamochi_ibraki_graph(const labelling& labelling, size_t k);
	std::vector<std::vector<vertex>> nagamochi_ibraki_adj_list(const labelling& labelling, size_t k);
	labelling nagamochi_ibraki_labelling(const std::vector<std::vector<vertex>>& adj_list);
}

