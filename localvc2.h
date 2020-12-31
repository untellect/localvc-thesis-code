#pragma once

#include <unordered_set>
#include "Graph2.h"
#include "Localvc.h"

namespace localvc2 {
	using namespace resettable_graph;
	typedef std::vector<std::vector<vertex>> adjlist;

	typedef bool(localec_fn)(resettable_graph::graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut);

	localec_fn local_edge_connectivity_v2, local_edge_connectivity_degreecount, local_edge_connectivity_marking_degreecount, local_edge_connectivity_marking2_degreecount ,local_edge_connectivity_stack, local_edge_connectivity_minors, local_edge_connectivity_v1;

	typedef bool(globalvc_ud_fn)(const adjlist& g, resettable_graph::graph& sg, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& cut); // undirected version
	globalvc_ud_fn vertex_connectivity_unbalanced_undirected_edge_sampling, vertex_connectivity_unbalanced_undirected_finish_edge_sampling, vertex_connectivity_unbalanced_skip;

	typedef localvc::globalvc_balanced_fn globalvc_balanced_fn;

	typedef bool (VC_fn)(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut);
	VC_fn VC_undirected_edge_sampling, VC_undirected_finish_edge_sampling, VC_undirected_edge_sampling_a_by_timing, VC_undirected_edge_sampling_trivial_separately, VC_undirected_finish_edge_sampling_trivial_separately;

	typedef std::unordered_set<vertex>(find_VC_fn)(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list);
	find_VC_fn find_first_vertex_connectivity_undirected, find_and_improve_vertex_connectivity_undirected;
	std::unordered_set<vertex> find_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, double a_coeff = 2.0 / 3.0, size_t boost = 0);
}