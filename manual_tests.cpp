#include "pch.h"

#define VERBOSE_PRINTING

//#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

#include "Util.h"
#include "Sparse.h"
#include "GraphIO.h"
#include "Localec-templated.h"
#include "MaxFlow.h"

#include "Mains.h"
#include "Benchmark.h"
#include "Benchmarker.h"
#include "preprocessing.h"

void time_localec(versioned_graph::graph& g_split, size_t m, size_t k, size_t s, size_t boost_mult, edge_source_sampler& sampler, unordered_set<vertex>& cut, localvc::localec_fn localec) {
	const size_t num_iterations = (m - 1) / s + 1;

	vector<unordered_set<vertex>> cuts;

	auto t0 = chrono::high_resolution_clock::now();
	for (size_t boost = 0; boost < boost_mult; boost++) { // boost 10 times to average out the time better.
		for (size_t i = 0; i < num_iterations; i++) {
			vertex x = sampler.get_vertex();
			vertex x_out = 2 * x + 1;
			size_t volume = s; // Multiply by at least 2.
			g_split.reset();
			bool found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
				g_split.reset();
				unordered_set<vertex> S_split = get_neighbors(g_split, cut);
				unordered_set<vertex> S;
				for (vertex v_split : S_split) {
					S.insert(v_split / 2);
				}
				cuts.push_back(S);
				//cout << "Found a cut." << endl;
			}
		}
	}
	auto t1 = chrono::high_resolution_clock::now();
	auto dt = (t1 - t0) / boost_mult;
	chrono::microseconds dt_micro = chrono::duration_cast<microseconds>(dt);
	cout << "vol:\t" << s << "\titers:\t" << num_iterations << "\tmicroseconds:\t" << dt_micro.count() << "\tsuccesses:\t" << cuts.size() << "/" << boost_mult * num_iterations << std::endl;
}

void time_localec2(resettable_graph::graph& g_split, size_t m, size_t k, size_t s, size_t boost_mult, edge_source_sampler& sampler, unordered_set<vertex>& cut, localvc2::localec_fn localec) {
	const size_t num_iterations = (m - 1) / s + 1;

	vector<unordered_set<vertex>> cuts;

	auto t0 = chrono::high_resolution_clock::now();
	for (size_t boost = 0; boost < boost_mult; boost++) { // boost 10 times to average out the time better.
		for (size_t i = 0; i < num_iterations; i++) {
			vertex x = sampler.get_vertex();
			vertex x_out = 2 * x + 1;
			size_t volume = s; // Multiply by at least 2.
			g_split.reset();
			bool found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
				g_split.reset();
				unordered_set<vertex> S_split = get_neighbors(g_split, cut);
				unordered_set<vertex> S;
				for (vertex v_split : S_split) {
					S.insert(v_split / 2);
				}
				cuts.push_back(S);
				//cout << "Found a cut." << endl;
			}
		}
	}
	auto t1 = chrono::high_resolution_clock::now();
	auto dt = (t1 - t0) / boost_mult;
	chrono::microseconds dt_micro = chrono::duration_cast<microseconds>(dt);
	cout << "vol:\t" << s << "\titers:\t" << num_iterations << "\tmicroseconds:\t" << dt_micro.count() << "\tsuccesses:\t" << cuts.size() << "/" << boost_mult * num_iterations << std::endl;
}

void time_maxflow(const adj_list& g, size_t m, size_t k, size_t boost_mult, edge_source_sampler& sampler, maxflow::maxflow_adj_fn* fn, string id) {
	const size_t num_iterations = k;

	auto t0 = chrono::high_resolution_clock::now();
	{
		for (size_t i = 0; i < num_iterations * boost_mult; i++) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(g);

			size_t k_xy = fn(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(g, L);
				//cout << "Found a cut." << endl;
			}
		}
	}
	auto t1 = chrono::high_resolution_clock::now();
	auto dt = (t1 - t0) / boost_mult;
	chrono::microseconds dt_micro = chrono::duration_cast<microseconds>(dt);
	cout << "vol:\t" << m / k << "\titers:\t" << num_iterations << "\tmicroseconds:\t" << dt_micro.count() << "\t (" << id << ")" << std::endl;
}

// Based on: vertex_connectivity_unbalanced_undirected_edge_sampling
void time_all_levels_of_localec(string id, const adj_list& g, versioned_graph::graph& g_split, localvc::localec_fn localec, size_t k, size_t boost_mult = 10) {
	size_t n = g_split.size_vertices() / 2;
	size_t m = g_split.size_edges() - n;

	std::unordered_set<vertex> cut;
	bool found_cut;
	auto sampler = edge_source_sampler(g);

	size_t d_min = SIZE_MAX;
	for (auto& vec : g) {
		if (vec.size() < d_min)
			d_min = vec.size();
	}

	cout << "id:\t" << id << std::endl;
	cout << "n:\t" << n << std::endl;
	cout << "m:\t" << m << std::endl;
	cout << "d_min:\t" << d_min << std::endl;
	cout << "k:\t" << k << std::endl;

	for (size_t s = 1; s < m / k; s *= 2) {
		time_localec(g_split, m, k, s, boost_mult, sampler, cut, localec);
	}

	time_maxflow(g, m, k, boost_mult, sampler, maxflow::ford_fulkerson_adj, "Ford-Fulkerson");
	time_maxflow(g, m, k, boost_mult, sampler, maxflow::ford_fulkerson_dfs_adj, "Ford-Fulkerson DFS");
	time_maxflow(g, m, k, boost_mult, sampler, maxflow::edmonds_karp, "Edmonds-Karp");
	time_maxflow(g, m, k, boost_mult, sampler, maxflow::dinics, "Dinic's");
}

// Based on: vertex_connectivity_unbalanced_undirected_edge_sampling
void time_all_levels_of_localec(string id, const adj_list& g, resettable_graph::graph& g_split, localvc2::localec_fn localec, size_t k, size_t boost_mult = 10) {
	size_t n = g.size();
	size_t m = 0;
	size_t d_min = SIZE_MAX;
	for (auto& vec : g) {
		m += vec.size();
		if (vec.size() < d_min)
			d_min = vec.size();
	}

	std::unordered_set<vertex> cut;
	bool found_cut;
	auto sampler = edge_source_sampler(g);

	cout << "id:\t" << id << std::endl;
	cout << "n:\t" << n << std::endl;
	cout << "m:\t" << m << std::endl;
	cout << "d_min:\t" << d_min << std::endl;
	cout << "k:\t" << k << std::endl;

	for (size_t s = 1; s < m / k; s *= 2) {
		time_localec2(g_split, m, k, s, boost_mult, sampler, cut, localec);
	}

	time_maxflow(g, m, k, boost_mult, sampler, maxflow::ford_fulkerson_adj, "Ford-Fulkerson");
	time_maxflow(g, m, k, boost_mult, sampler, maxflow::ford_fulkerson_dfs_adj, "Ford-Fulkerson DFS");
	time_maxflow(g, m, k, boost_mult, sampler, maxflow::edmonds_karp, "Edmonds-Karp");
	time_maxflow(g, m, k, boost_mult, sampler, maxflow::dinics, "Dinic's");
}

void test0() {
	size_t L = 5;
	size_t S = 30;
	size_t R = 1000 - L - S;
	size_t d = 60;
	size_t k = S; // NO-case is simpler.
	//k = S + 1; // YES-case
	localvc::localec_fn* localec1 = localvc::local_edge_connectivity_v2;
	localvc2::localec_fn* localec2 = localvc2::local_edge_connectivity_v2;
	localvc2::localec_fn* localec2_deg = localvc2::local_edge_connectivity_degreecount;
	localvc2::localec_fn* localec2_mdeg2 = localvc2::local_edge_connectivity_marking2_degreecount;

	adj_list adj;
	if (true) {
		adj = versioned_graph::make_undirected_FG_LSR_graph_adj_list(L, S, R, d);
	}
	else if (true) {
		ifstream in;
		in.open("C:/Users/Public/Documents/RHG/13/13-5-10+4.txt");
		adj = read_adjlist_from_edgelist_undirected(in);
		k = 7;
	}

	auto labelling = sparse::nagamochi_ibraki_labelling(adj);
	adj_list sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
	versioned_graph::graph g_split_1 = make_split_graph(sparsified);
	resettable_graph::graph g_split_2(make_split_graph_adj_list(sparsified));



	//time_all_levels_of_localec("local1", sparsified, g_split_1, localec1, k);
	//std::cout << std::endl;
	//time_all_levels_of_localec("local2", sparsified, g_split_2, localec2, k);
	//std::cout << std::endl;
	//time_all_levels_of_localec("deg", sparsified, g_split_2, localec2_deg, k);
	//std::cout << std::endl;
	time_all_levels_of_localec("mdeg", sparsified, g_split_2, localec2_mdeg2, k, 10);
}