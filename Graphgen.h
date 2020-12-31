#pragma once

#include "Graph.h"
#include "Graph2.h"

namespace versioned_graph {
	typedef std::vector<std::vector<vertex>> adj_list;

	graph make_complete_graph(size_t n);
	graph make_randomised_complete_graph(size_t n);
	adj_list make_sparsified_complete_LSR_adj_list(size_t n_1, size_t n_2, size_t n_3);
	graph make_complete_LSR_graph(size_t n_1, size_t n_2, size_t n_3);
	graph make_sparsified_complete_LSR_graph(size_t n_1, size_t n_2, size_t n_3);
	adj_list make_randomised_complete_LSR_graph_adj_list(size_t n_1, size_t n_2, size_t n_3);
	graph make_randomised_complete_LSR_graph(size_t n_1, size_t n_2, size_t n_3);
	graph make_exact_degree_LSR_multigraph(size_t n_1, size_t n_2, size_t n_3, size_t d);
	adj_list make_exact_degree_LSR_multigraph_adj_list(size_t n_1, size_t n_2, size_t n_3, size_t d);
	adj_list make_undirected_min_degree_LSR_graph_adj_list(size_t n_1, size_t n_2, size_t n_3, size_t d);
	adj_list make_undirected_FG_LSR_graph_adj_list(size_t n_1, size_t n_2, size_t n_3, size_t k);

	adj_list make_randomised_LSR_erdos_renyi(size_t n_1, size_t n_2, size_t n_3, double p_in, double p_out);
	adj_list make_randomised_undirected_LSR_erdos_renyi(size_t n_1, size_t n_2, size_t n_3, double p_in, double p_out);

	std::vector<std::vector<vertex>> make_split_graph_adj_list(const std::vector<std::vector<vertex>>& orig);
	graph make_split_graph(const graph& orig);
	graph make_reverse_graph(const graph& orig);
	graph make_sparse_graph(const graph& orig, size_t k);
}

namespace resettable_graph {
	graph make_split_graph(const graph& orig);
}