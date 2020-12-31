#pragma once

#include <vector>
#include <unordered_set>

#include "localvc.h"

namespace preprocessing {
	typedef localvc::adj_list adj_list;

	// These seem to work.
	adj_list get_subgraph(adj_list& G, std::vector<size_t> V); // Gets the subgraph G(V) with all the indices shifted.
	std::vector<size_t> kcore_vertices_undirected(adj_list& G, size_t k); // Gets the vertices that are included in the k-core. G must be undirected/bidirectional
	adj_list LCC_of_kcore(adj_list& G, size_t k);
	
	// These might not.
	void in_place_subgraph(adj_list& G, std::vector<size_t>& V);
	void in_place_k_core_undirected(adj_list& G, size_t k);
	void in_place_LWCC(adj_list& G);
	void in_place_LCC_of_kcore(adj_list& G, size_t k);

	std::vector<std::vector<size_t>> all_CC_vertices_undirected(adj_list& G);
	std::vector<size_t> LCC_vertices_undirected(adj_list& G);
	std::vector<adj_list> CC_after_cut(adj_list& G, std::unordered_set<size_t> separator);

	void inplace_remove_duplicate_edges(adj_list& G);

	// This definitely doesn't.
	std::vector<adj_list> k_connected_components(adj_list& G, size_t k); // Using localvc with default choices
}