#pragma once

#include <unordered_set>
#include "Graph3.h"
#include "Localvc.h"

namespace localvc3 {
	using namespace partial_graph;
	typedef std::vector<std::vector<vertex>> adjlist;

	typedef bool(localec_fn)(graph& sg, vertex x, size_t nu, size_t k, std::unordered_set<vertex>& cut);

	localec_fn local_edge_connectivity_v8;

	typedef bool(globalvc_ud_fn)(const adjlist& g, graph& sg, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& cut); // undirected version
	globalvc_ud_fn vertex_connectivity_unbalanced_undirected_edge_sampling;

	typedef localvc::globalvc_balanced_fn globalvc_balanced_fn;

	typedef bool (VC_fn)(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut);
	VC_fn VC_undirected_edge_sampling;
	//VC_fn VC_undirected_edge_sampling_trivial_separately;

	std::unordered_set<vertex> find_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t boost = 0);
}