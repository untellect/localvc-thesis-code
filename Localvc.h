#pragma once

#include "Graph.h"
#include <unordered_set>
#include "Graphgen.h"

namespace localvc {
	using namespace versioned_graph;

	// If true, then we found a cut and set the parameter 'cut' to it.
	typedef bool(localec_fn)(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut);

	// Page 35 version (and variations)
	localec_fn local_edge_connectivity_v1, local_edge_connectivity_v2, local_edge_connectivity_v7;
	// Page 10 version (and variations)
	localec_fn local_edge_connectivity_v3, local_edge_connectivity_v4, local_edge_connectivity_v5, local_edge_connectivity_v6, local_edge_connectivity_v8;

	struct globalvc_graphs {
		graph g;
		graph g_split;
		graph g_rev;
		graph g_rev_split;
		globalvc_graphs(graph& graph): g(graph) {
			g_split = make_split_graph(g);
			g_rev = make_reverse_graph(g);
			g_rev_split = make_split_graph(g_rev);
		}
	};

	typedef bool(globalvc_ud_fn)(const adj_list& g, graph& g_split, size_t k, size_t min_volume, size_t max_volume, localec_fn localec, vertex& x, std::unordered_set<vertex>& cut); // undirected version
	globalvc_ud_fn vertex_connectivity_unbalanced_undirected_vertex_sampling;
	globalvc_ud_fn vertex_connectivity_unbalanced_undirected_mixed2_hybrid_sampling;
	globalvc_ud_fn vertex_connectivity_unbalanced_undirected_edge_sampling;
	globalvc_ud_fn vertex_connectivity_unbalanced_undirected_edge_sampling_from_2dmin;
	//globalvc_ud_fn vertex_connectivity_unbalanced_undirected_edge_sampling_to_a;
	globalvc_ud_fn nonstop_vertex_connectivity_unbalanced_undirected_hybrid_sampling;
	typedef std::pair<std::unordered_set<vertex>, bool>(globalvc_fn)(globalvc_graphs& g, size_t k, size_t a, localec_fn localec);
	globalvc_fn vertex_connectivity_unbalanced_vertex_sampling;
	globalvc_fn vertex_connectivity_unbalanced_mixed_vertex_sampling;
	globalvc_fn vertex_connectivity_unbalanced_hybrid_sampling;
	globalvc_fn vertex_connectivity_unbalanced_small_hybrid_sampling;
	globalvc_fn vertex_connectivity_unbalanced_mixed2_hybrid_sampling;
	globalvc_fn vertex_connectivity_unbalanced_mixed_hybrid_sampling;
	globalvc_fn vertex_connectivity_unbalanced_mixed_hybrid_sampling_skiplow;
	globalvc_fn vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling;
	globalvc_fn vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling_skiplow;
	globalvc_fn vertex_connectivity_unbalanced_edge_sampling;
#ifdef WIN32
	globalvc_fn vertex_connectivity_unbalanced_hybrid_lp;
#endif

	typedef bool (globalvc_balanced_fn)(const std::vector<std::vector<vertex>>& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut);
	globalvc_balanced_fn vertex_connectivity_balanced_vertex_sampling;
	globalvc_balanced_fn vertex_connectivity_balanced_vertex_sampling_adj_list;
	globalvc_balanced_fn vertex_connectivity_balanced_edge_sampling;
	globalvc_balanced_fn vertex_connectivity_balanced_edge_sampling_adj_list;
	globalvc_balanced_fn vertex_connectivity_balanced_finish_edge_sampling_adj_list;
	globalvc_balanced_fn vertex_connectivity_balanced_dinic_edge_sampling_adj_list;
	globalvc_balanced_fn nonstop_vertex_connectivity_balanced_vertex_sampling;
	globalvc_balanced_fn vertex_connectivity_push_relabel_edge_sampling;

	//size_t find_vertex_connectivity_undirected_binarysearch(const std::vector<std::vector<vertex>>& adj_list, globalvc_ud_fn globalvc_ud, localec_fn localec, double a_coeff, vertex& x, std::unordered_set<vertex>& min_cut, size_t boost = 100);
	//size_t find_vertex_connectivity_undirected_no_binarysearch(const std::vector<std::vector<vertex>>& adj_list, globalvc_ud_fn globalvc_ud, localec_fn localec, double a_coeff, vertex& x, std::unordered_set<vertex>& min_cut, size_t boost = 100);
	
	void find_vertex_connectivity_undirected_hybrid_sampling(const std::vector<std::vector<vertex>>& adj_list, localec_fn localec, double a_coeff, vertex& x, std::unordered_set<vertex>& min_cut, size_t boost = 1);
	void find_vertex_connectivity_undirected_mixed2_hybrid_sampling(const std::vector<std::vector<vertex>>& adj_list, localec_fn localec, double a_coeff, vertex& x, std::unordered_set<vertex>& min_cut, size_t boost = 1);
	
	typedef bool (VC_fn)(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, double a_coeff, std::unordered_set<vertex>& cut);
	VC_fn VC_undirected_edge_sampling;
	VC_fn VC_undirected_vertex_sampling;
	VC_fn VC_undirected_edge_sampling_a_by_timing;
	VC_fn VC_undirected_edge_sampling_trivial_separately;
	//bool VC_undirected(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, graph& g, graph& g_split, size_t k, size_t a, vertex& x, std::unordered_set<vertex>& cut);
	std::unordered_set<vertex> find_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, double a_coeff = 2.0/3.0, size_t boost = 0);


}
