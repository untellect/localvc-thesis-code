#include "pch.h"

#include "Benchmark.h"

#include "MaxFlow.h"
#include "PairPreflowPush.h"
#include "Util.h"
#include "Henzinger.h"

#include <fstream>
#include "GraphIO.h"


void benchmark_adj(const adj_list& adjacency_list, size_t N, size_t boost, double a_coeff) {
	VC_fn* vc = VC_undirected_edge_sampling;
	localec_fn* localec = local_edge_connectivity_v2;
	globalvc_ud_fn* unbalancedvc = vertex_connectivity_unbalanced_undirected_edge_sampling;
	globalvc_balanced_fn* balancedvc = vertex_connectivity_push_relabel_edge_sampling;
	globalvc_balanced_fn* balancedvc2 = vertex_connectivity_balanced_edge_sampling;
	globalvc_balanced_fn* balancedvc3 = vertex_connectivity_balanced_edge_sampling_adj_list;
	henzinger::henzinger_vc_fn* henzingervc = henzinger::vc_undirected_linear_unsparsified_random;

	//benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2");
	//benchmark_ud(adjacency_list, a_coeff, N, boost, unbalancedvc, localec, "unbalanced undirected mixed 2 hybrid 2");

	//benchmark_ud2(adjacency_list, a_coeff, N, boost, localec, "unbalanced undirected hybrid 2");
	//benchmark_VC_undirected(vc, local_edge_connectivity_v2, unbalancedvc, balancedvc, adjacency_list, N);
	benchmark_henzinger(henzingervc, adjacency_list, N);
	for(size_t b = 0; b <= boost; b++)
		benchmark_find_VC_undirected(VC_undirected_edge_sampling, localec, unbalancedvc, balancedvc3, adjacency_list, a_coeff, b, N, "edge");
}

void benchmark_file(string filename, size_t N, size_t boost, double a_coeff) {
	ifstream in;
	in.tie(NULL);
	in.open(filename);

	auto t_0 = high_resolution_clock::now();
	std::vector<std::vector<vertex>> adjacency_list = read_adjlist_from_edgelist_undirected(in);
	graph g(adjacency_list);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms to read the file." << endl;
	//cout << g;
	cout << "G has " << g.size_vertices() << " vertices and " << g.size_edges() << " edges." << endl;

	benchmark_adj(adjacency_list, N, boost, a_coeff);
}

void benchmark_file2(string filename, size_t N, size_t boost, double a_coeff, string id) {
	ifstream in;
	in.tie(NULL);
	in.open(filename);

	auto t_0 = high_resolution_clock::now();
	std::vector<std::vector<vertex>> adjacency_list = read_adjlist_from_edgelist_undirected(in);
	graph g(adjacency_list);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

	//cout << "It took " << dt.count() << " ms to read the file." << endl;
	//cout << "G has " << g.size_vertices() << " vertices and " << g.size_edges() << " edges." << endl;


	VC_fn* vc = VC_undirected_edge_sampling;
	localec_fn* localec = local_edge_connectivity_v2;
	globalvc_ud_fn* unbalancedvc = vertex_connectivity_unbalanced_undirected_edge_sampling;
	globalvc_balanced_fn* balancedvc = vertex_connectivity_push_relabel_edge_sampling;
	globalvc_balanced_fn* balancedvc2 = vertex_connectivity_balanced_edge_sampling;
	globalvc_balanced_fn* balancedvc3 = vertex_connectivity_balanced_edge_sampling_adj_list;

	//benchmark_henzinger(adjacency_list, N);
	benchmark_find_VC_undirected(vc, localec, unbalancedvc, balancedvc, adjacency_list, a_coeff, boost, N, id);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void benchmark_localec(graph& g, vertex x, size_t nu, size_t k, size_t N, localec_fn localec, string id) {
	size_t successes = 0;
	cout << "Starting " << N << " iterations of localec " << id << endl;
	auto t_0 = high_resolution_clock::now();
	for (size_t i = 0; i < N; ++i) {
		g.reset();

		unordered_set<vertex> cut;
		bool found_cut;

		found_cut = localec(g, x, nu, k, cut);
		if (found_cut)
			++successes;
	}
	auto t_1 = chrono::high_resolution_clock::now();
	cout << "Finished " << N << " iterations of localec " << id << endl;
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms." << endl;
	double p = (double)successes / (double)N;
	cout << "Success count: " << successes << " (" << 100 * p << "%)" << endl;
	cout << "Calls that return a cut are considered successful." << endl;
	double projected_repetitions = log(p) / log(0.999);
	auto projected_time = (dt * projected_repetitions) / N;
	cout << "Estimated time for 99.9% accuracy: " << projected_time.count() << " ms." << endl;
	cout << "---" << endl;
}

void benchmark_localec_multi_nu(graph& g, vertex x, size_t base_nu, size_t k, size_t N, localec_fn localec, string id, vector<double> d_i) {
	unordered_set<vertex> cut;
	bool found_cut;

	cout << "N\t*nu\tt(ms)\tN_+\t%_+\tvol\tid\tt_99.9%" << endl;

	for (double i : d_i) {
		size_t nu = (size_t)(i * base_nu);
		size_t successes = 0;

		size_t vol = 0;

		auto t_0 = high_resolution_clock::now();
		for (size_t i = 0; i < N; ++i) {
			g.reset();
			found_cut = localec(g, x, nu, k, cut);
			if (found_cut) {
				++successes;
				size_t vol2 = get_reachable_volume(g, x);
				if (vol < vol2)
					vol = vol2;
			}
		}
		auto t_1 = chrono::high_resolution_clock::now();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
		double p = (double)successes / (double)N;
		double projected_repetitions = log(0.001) / log(1-p);
		if (p >= 0.999) projected_repetitions = 1;
		auto projected_time = (dt * projected_repetitions) / N;

		cout << N << "\t" << i << "\t" << dt.count() << "\t" << successes << "\t" << 100 * p << "\t" << vol << "\t" << id << "\t" << projected_time.count() << endl;
	}
}

void benchmark_localec2_multi_nu(localvc2::graph& g, vertex x, size_t base_nu, size_t k, size_t N, localvc2::localec_fn localec, string id, vector<double> d_i) {
	unordered_set<vertex> cut;
	bool found_cut;

	cout << "N\t*nu\tt(ms)\tN_+\t%_+\tvol\tid\tt_99.9%" << endl;

	for (double i : d_i) {
		size_t nu = (size_t)(i * base_nu);
		size_t successes = 0;

		size_t vol = 0;

		auto t_0 = high_resolution_clock::now();
		for (size_t i = 0; i < N; ++i) {
			g.reset();
			found_cut = localec(g, x, nu, k, cut);
			if (found_cut) {
				++successes;
				size_t vol2 = get_reachable_volume(g, x);
				if (vol < vol2)
					vol = vol2;
			}
		}
		auto t_1 = chrono::high_resolution_clock::now();
		auto dt = t_1 - t_0;
		double p = (double)successes / (double)N;
		double projected_repetitions = log(0.001) / log(1 - p); // (1-p)^projected_repetitions = 0.001 = probability of failure
		if (p >= 0.999) projected_repetitions = 1;
		auto projected_time = (dt * projected_repetitions) / N;

		auto dt_ms = duration_cast<std::chrono::milliseconds>(dt).count();
		auto projected_time_ms = duration_cast<std::chrono::milliseconds>(projected_time).count();

		cout << N << "\t" << i << "\t" << dt_ms << "\t" << successes << "\t" << 100 * p << "\t" << vol << "\t" << id << "\t" << projected_time_ms << endl;
	}
}

void benchmark_globalvc(graph& g, size_t k, size_t a, size_t N, globalvc_fn globalvc, localec_fn localec, string id) {
	size_t successes = 0;
	cout << "Starting " << N << " iterations of globalvc " << id << endl;
	auto t_0 = high_resolution_clock::now();
	globalvc_graphs graphs(g);
	for (size_t i = 0; i < N; ++i) {
		auto cutpair = globalvc(graphs, k, a, localec);
		if (cutpair.second == false) { // cutpair.second == false iff we found a cut.
			++successes;
		}
	}
	auto t_1 = chrono::high_resolution_clock::now();
#ifdef VERBOSE_PRINTING
	cout << "Finished " << N << " iterations of globalvc " << id << endl;
#endif
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms. (" << dt.count() / N << " ms per call)" << endl;
	double p = (double)successes / (double)N;
#ifndef VERBOSE_PRINTING
	if (successes != 0 && successes != N)
#endif
	{
		cout << "Success count: " << successes << " (" << 100 * p << "%)" << endl;
		cout << "Calls that return a cut are considered successful." << endl;
		double projected_repetitions = log(p) / log(0.999);
		auto projected_time = (dt * projected_repetitions) / N;
		cout << "Estimated time for 99.9% accuracy: " << projected_time.count() << " ms." << endl;
	}
	cout << "---" << endl;
}

/*
void benchmark_ud(const std::vector<std::vector<vertex>>& adjacency_list, double a_coeff, size_t N, size_t boost, globalvc_ud_fn globalvc, localec_fn localec, string id) {
	cout << "Starting " << N << " iterations of find-globalvc " << id << " (" << boost << "x boost)" << endl;

	cout << "k\tt(ms)\tv\tx" << endl;
	for (size_t i = 0; i < N; ++i) {
		vertex x;
		std::unordered_set<vertex> min_cut;

		auto t_0 = high_resolution_clock::now();
		size_t k = find_vertex_connectivity_undirected_no_binarysearch(adjacency_list, globalvc, localec, a_coeff, x, min_cut, boost);
		auto t_1 = high_resolution_clock::now();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		graph g = graph(adjacency_list);
		size_t v = get_vertex_cut_volume(g, x, min_cut);

		cout << k << "\t" << dt.count() << "\t" << v << "\t" << x << endl;
	}
}
*/

void benchmark_ud2(const std::vector<std::vector<vertex>>& adjacency_list, double a_coeff, size_t N, size_t boost, localec_fn localec, string id) {
	cout << "Starting " << N << " iterations of find-globalvc " << id << " (" << boost << "x boost)" << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < N; ++i) {
		vertex x = 0;
		std::unordered_set<vertex> min_cut;
		auto t_0 = high_resolution_clock::now();
		find_vertex_connectivity_undirected_mixed2_hybrid_sampling(adjacency_list, localec, a_coeff, x, min_cut, boost);
		auto t_1 = high_resolution_clock::now();
		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\tms" << endl;

#if false
		cout << "min_cut:";
		vector<vertex> cut(min_cut.begin(), min_cut.end());
		sort(cut.begin(), cut.end());
		for (vertex v : cut)
			cout << " " << v;
		cout << endl;
#endif
	}
}

/*
void benchmark_ud3(graph& g, size_t k, size_t a, size_t N, globalvc_ud_fn globalvc, localec_fn localec, string id) {
	size_t successes = 0;
	cout << "Starting " << N << " iterations of globalvc " << id << endl;
#ifdef VERBOSE_PRINTING
	cout << "a: " << a << endl;
#endif
	auto t_0 = high_resolution_clock::now();
	graph g_split = make_split_graph(g);
	for (size_t i = 0; i < N; ++i) {
		vertex x;
		unordered_set<vertex> cut;
		bool found_cut = globalvc(g, g_split, k, a, localec, x, cut);
		if (found_cut) {
#ifdef PRINTCUTVOLUME
			std::cout << "Cut volume: " << get_vertex_cut_volume(g, x, cut) << std::endl;
#endif
			++successes;
		}
	}
	auto t_1 = chrono::high_resolution_clock::now();
#ifdef VERBOSE_PRINTING
	cout << "Finished " << N << " iterations of globalvc " << id << endl;
#endif
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms. (" << dt.count() / N << " ms per call)" << endl;
	double p = (double)successes / (double)N;
#ifndef VERBOSE_PRINTING
	if (successes != 0 && successes != N)
#endif
	{
		cout << "Success count: " << successes << " (" << 100 * p << "%)" << endl;
		cout << "Calls that return a cut are considered successful." << endl;
		double projected_repetitions = log(p) / log(0.999);
		auto projected_time = (dt * projected_repetitions) / N;
		cout << "Estimated time for 99.9% accuracy: " << projected_time.count() << " ms." << endl;
	}
	cout << "---" << endl;
}
*/


void benchmark_VC_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adjacency_list, size_t N) {
	cout << "Starting " << N << " iterations of VC_undirected " << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < N; ++i) {
		auto t_0 = high_resolution_clock::now();
		std::unordered_set<vertex> min_cut;
		size_t max_k = 20;
		bool found_cut = vc(localec, unbalancedvc, balancedvc, adjacency_list, max_k, 2.0 / 3.0, min_cut);
		auto t_1 = high_resolution_clock::now();
		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\tms" << endl;
	}
}

void benchmark_find_VC_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adjacency_list, double a_coeff, size_t boost, size_t N, string id)
{
	cout << "Starting " << N << " iterations of localbased-VC id " << id << endl;
	cout << "k\tt(ms)\tboost\tcut" << endl;
	for (size_t i = 0; i < N; ++i) {
		auto t_0 = high_resolution_clock::now();
		std::unordered_set<vertex> min_cut = find_vertex_connectivity_undirected(vc, localec, unbalancedvc, balancedvc, adjacency_list, a_coeff, boost);
		auto t_1 = high_resolution_clock::now();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
		size_t k = min_cut.size();

		cout << min_cut.size() << "\t" << dt.count() << "\t" << boost;

		cout << "\t";
		for (vertex v : min_cut)
			cout << v << " ";
		cout << endl;
	}
}

void benchmark_pair_edmonds_karp(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max) {
	cout << "Starting " << s_t_pairs.size() << " iterations of edmonds-karp (vector based) pairwise connectivity " << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < s_t_pairs.size(); ++i) {
		std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adjacency_list);

		auto t_0 = high_resolution_clock::now();

		std::unordered_set<vertex> min_cut;

		size_t s_out = s_t_pairs[i].first * 2 + 1;
		size_t t_in = s_t_pairs[i].second * 2;
		size_t k_st = maxflow::edmonds_karp(g_split, s_out, t_in, k_max);

		auto t_1 = high_resolution_clock::now();

		if (k_st < k_max) {
			std::unordered_set<vertex> L_split = get_reachable(g_split, s_out);
			std::unordered_set<vertex> L;
			for (vertex v_split : L_split) {
				if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
					L.insert(v_split / 2);
			}
			min_cut = get_neighbors(adjacency_list, L);
		}
		auto t_2 = high_resolution_clock::now();

		size_t k = min_cut.size();
		auto dt_1 = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
		auto dt_2 = duration_cast<std::chrono::milliseconds>(t_2 - t_0);

		cout << min_cut.size() << "\t" << dt_2.count() << "\tms" << endl;
		//cout << "cut:";
		//for (size_t v : min_cut) cout << " " << v;
		//cout << endl;
	}
}

void benchmark_pair_fordfulkerson(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max) {
	cout << "Starting " << s_t_pairs.size() << " iterations of ford-fulkerson (custom graph) pairwise connectivity " << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < s_t_pairs.size(); ++i) {
		versioned_graph::graph g(adjacency_list);
		versioned_graph::graph g_split = make_split_graph(adjacency_list);
		auto t_0 = high_resolution_clock::now();

		std::unordered_set<vertex> min_cut;

		size_t s_out = s_t_pairs[i].first * 2 + 1;
		size_t t_in = s_t_pairs[i].second * 2;
		size_t k_st = maxflow::ford_fulkerson(g_split, s_out, t_in, k_max);

		if (k_st < k_max) {
			std::unordered_set<vertex> L_split = get_reachable(g_split, s_out);
			std::unordered_set<vertex> L;
			for (vertex v_split : L_split) {
				if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
					L.insert(v_split / 2);
			}
			min_cut = get_neighbors(g, L);
		}
		auto t_1 = high_resolution_clock::now();

		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\tms" << endl;
		//cout << "cut:";
		//for (size_t v : min_cut) cout << " " << v;
		//cout << endl;
	}
}



void benchmark_pair_fordfulkerson2(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max) {
	cout << "Starting " << s_t_pairs.size() << " iterations of ford-fulkerson (vector based) pairwise connectivity (up to k=" << k_max << ")" << endl;
	cout << "k\tt(ms)\ts\tt" << endl;
	for (size_t i = 0; i < s_t_pairs.size(); ++i) {
		std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adjacency_list);

		auto t_0 = high_resolution_clock::now();

		std::unordered_set<vertex> min_cut;

		size_t s_out = s_t_pairs[i].first * 2 + 1;
		size_t t_in = s_t_pairs[i].second * 2;
		size_t k_st = maxflow::ford_fulkerson_adj(g_split, s_out, t_in, k_max);

		if (k_st < k_max) {
			std::unordered_set<vertex> L_split = get_reachable(g_split, s_out);
			std::unordered_set<vertex> L;
			for (vertex v_split : L_split) {
				if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
					L.insert(v_split / 2);
			}
			min_cut = get_neighbors(adjacency_list, L);
		}
		auto t_1 = high_resolution_clock::now();

		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\t" << s_t_pairs[i].first << "\t" << s_t_pairs[i].second << endl;
		//cout << "cut:";
		//for (size_t v : min_cut) cout << " " << v;
		//cout << endl;
	}
}

/*
void benchmark_pair_fordfulkerson3(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max) {
	cout << "Starting " << s_t_pairs.size() << " iterations of ford-fulkerson (list based) pairwise connectivity " << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < s_t_pairs.size(); ++i) {
		std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adjacency_list);

		auto t_0 = high_resolution_clock::now();

		size_t s_out = s_t_pairs[i].first * 2 + 1;
		size_t t_in = s_t_pairs[i].second * 2;
		std::unordered_set<vertex> min_cut = maxflow::ford_fulkerson_list(g_split, s_out, t_in, k_max);

		auto t_1 = high_resolution_clock::now();

		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\tms" << endl;
		//cout << "cut:";
		//for (size_t v : min_cut) cout << " " << v;
		//cout << endl;
	}
}
*/

void benchmark_pair_preflow(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs) {
	cout << "Starting " << s_t_pairs.size() << " iterations of preflow pairwise connectivity " << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < s_t_pairs.size(); ++i) {
		size_t s = s_t_pairs[i].first;
		size_t t = s_t_pairs[i].second;

		std::unordered_set<vertex> min_cut;
		auto t_0 = high_resolution_clock::now();
		preflow::pair_vertex_connectivity(adjacency_list, s, t, min_cut);
		auto t_1 = high_resolution_clock::now();
		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\tms" << endl;
		//cout << "cut:";
		//for (size_t v : min_cut) cout << " " << v;
		//cout << endl;
	}
}

void benchmark_henzinger(henzinger::henzinger_vc_fn vc, const std::vector<std::vector<vertex>>& adjacency_list, size_t N, std::string id, bool verbose) {
	cout << "Starting " << N << " iterations of henzinger-rao-gabow id " << id << endl;
	cout << "k\tt(ms)\tx\tcut" << endl;
	for (size_t i = 0; i < N; ++i) {
		auto t_0 = high_resolution_clock::now();
		vertex x;
		//std::unordered_set<vertex> min_cut = henzinger::vc_undirected_linear_unsparsified_random(adjacency_list, x);
		std::vector<henzinger::henzinger_subresult> subres;
		std::unordered_set<vertex> min_cut = vc(adjacency_list, x, subres);
		auto t_1 = high_resolution_clock::now();
		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << "\t" << x;

		cout << "\t";
		for (vertex v : min_cut)
			cout << v << " ";
		cout << endl;
	}
}

void benchmark_henzinger_pair(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs) {
	cout << "Starting " << s_t_pairs.size() << " iterations of henzinger-rao-gabow (only one pair) " << endl;
	cout << "k\tt(ms)" << endl;
	for (size_t i = 0; i < s_t_pairs.size(); ++i) {
		size_t s = s_t_pairs[i].first;
		size_t t = s_t_pairs[i].second;
		auto t_0 = high_resolution_clock::now();
		std::unordered_set<vertex> min_cut = henzinger::min_vc_split_pair(adjacency_list, s, t);
		auto t_1 = high_resolution_clock::now();
		size_t k = min_cut.size();
		auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

		cout << min_cut.size() << "\t" << dt.count() << endl;
	}
}