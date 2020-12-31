#pragma once

#include "Graph.h"

#include <unordered_set>

namespace maxflow {
	using namespace versioned_graph;
	typedef std::vector<std::vector<vertex>> adjacency_list;

	size_t ford_fulkerson(graph& g, vertex s, vertex t, size_t k);

	typedef size_t(maxflow_adj_fn)(adjacency_list& g, vertex s, vertex t, size_t k);
	
	maxflow_adj_fn edmonds_karp, ford_fulkerson_adj, ford_fulkerson_dfs_adj, dinics;

	//size_t edmonds_karp(adjacency_list& g, vertex s, vertex t, size_t k);
	//size_t ford_fulkerson_adj(adjacency_list& rg, vertex s, vertex t, size_t k);
	//size_t ford_fulkerson_dfs_adj(adjacency_list& rg, vertex s, vertex t, size_t k);
	//size_t dinics(adjacency_list& rg, vertex s, vertex t, size_t k);

	//std::unordered_set<size_t> ford_fulkerson_list(adjacency_list& rg, vertex s, vertex t, size_t k);
}
