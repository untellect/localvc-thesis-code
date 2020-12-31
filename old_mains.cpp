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

#include "Mains.h"
#include "Benchmark.h"
#include "Benchmarker.h"
#include "preprocessing.h"

using namespace localvc;
using namespace std;
using namespace std::chrono;

string output_dir = "C:/Users/work/Documents/results/14/";

inline void configure_bm(UndirectedBenchmarker& bm) {
	bm.N = 5;

	bm.localvc_cfg[0].on = true; // LOCAL1
	bm.localvc_cfg[1].on = true; // LOCAL1DMIN

	bm.localvc2_cfg[0].on = true; // LOCAL2
	bm.localvc2_cfg[1].on = true; // LOCAL2DMIN
	bm.localvc2_cfg[2].on = true; // LOCAL2DEG
	bm.localvc2_cfg[3].on = true; // LOCAL2DEGDMIN
	bm.localvc2_cfg[4].on = true; // LOCAL2MARKDEG
	bm.localvc2_cfg[5].on = true; // LOCAL2MARK2DEG

	bm.henzinger_cfg[0].on = true; // HRG
}

void main0() {
	size_t N = 1;

	ifstream in;
	in.tie(NULL);
	string g1 = "C:/Users/Public/Documents/RHG/10-5-10+0.txt";
	string g2 = "C:/Users/Public/Documents/RHG/10-9-10.txt";
	string g3 = "C:/Users/Public/Documents/RHG/12-11-10.txt";

	std::vector<std::vector<vertex>> adjacency_list;

#if true
	auto t_0 = high_resolution_clock::now();
	adjacency_list = make_randomised_LSR_erdos_renyi(1000, 10, 1000, 0.1, 0.1);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms to generate the file." << endl;
	size_t m = 0;
	for (auto& vec : adjacency_list) m += vec.size();
	cout << "m: " << m << endl;
#else
	auto t_0 = high_resolution_clock::now();
	in.open(g2);
	adjacency_list = read_adjlist_from_edgelist_undirected(in);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms to read the file." << endl;
#endif

	graph g(adjacency_list);
	cout << "G has " << g.size_vertices() << " vertices and " << g.size_edges() << " edges." << endl;

	//benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2");
	//benchmark_ud(adjacency_list, a_coeff, N, boost, vertex_connectivity_unbalanced_undirected_edge_sampling, local_edge_connectivity_v2, "unbalanced undirected mixed 2 hybrid 2");
	//benchmark_ud2(adjacency_list, a_coeff, N, boost, local_edge_connectivity_v2, "unbalanced undirected hybrid 2");

	size_t n = g.size_vertices();

	std::vector<std::pair<size_t, size_t>> s_t_pairs;
	for (size_t i = 0; i < 10; i++) {
		size_t s = random_int(0, n - 1);
		size_t t = random_int(0, n - 1);
		if (s != t) {
			s_t_pairs.push_back({ s, t });
		}
	}

	size_t k = n;
	auto labelling = sparse::nagamochi_ibraki_labelling(adjacency_list);
	auto adjacency_list_2 = sparse::nagamochi_ibraki_adj_list(labelling, k);



	//benchmark_pair_edmonds_karp(adjacency_list_2, s_t_pairs, k);
	//benchmark_pair_fordfulkerson(adjacency_list_2, s_t_pairs, k); // custom

	//benchmark_pair_fordfulkerson2(adjacency_list_2, s_t_pairs, k); // vector

	//benchmark_pair_fordfulkerson3(adjacency_list_2, s_t_pairs, k); // list
	//benchmark_henzinger_pair(adjacency_list_2, s_t_pairs);
	//benchmark_pair_preflow(adjacency_list, s_t_pairs);

	for (size_t k = 1; k <= 16; k *= 2) {
		adjacency_list_2 = sparse::nagamochi_ibraki_adj_list(labelling, k);
		benchmark_pair_fordfulkerson2(adjacency_list_2, s_t_pairs, k); // vector
		benchmark_pair_preflow(adjacency_list_2, s_t_pairs);
	}

	benchmark_pair_preflow(adjacency_list, s_t_pairs);
	//benchmark_pair_fordfulkerson2(adjacency_list, s_t_pairs, n); // vector
}

void main1() {
	ios_base::sync_with_stdio(false);

	size_t N = 5;
	//double a_coeff = 4.0 / 6.0;
	//size_t boost = 3;

	string fn1 = "C:/Users/Public/Documents/RHG/";
	string fn2 = "-5-10+";
	string fn3 = ".txt";

	// Which graph sizes do we test on?
	for (size_t size_param : {10, 11, 12, 13, 14, 15}) {
		// Which instances (0-19) do we test on?
		for (size_t i = 0; i < 20; i++) {
			// What level of boosting do we test?
			for (double a_coeff : {4.0 / 6.0}) {
				for (size_t boost : {3}) {
					benchmark_file(fn1 + to_string(size_param) + fn2 + to_string(i) + fn3, N, boost, a_coeff);
				}
			}
		}
	}

	//benchmark_file("C:/Users/Public/Documents/RWG/web-NotreDame.txt", 1, 100);
	//benchmark_file("C:/Users/Public/Documents/RWG/web-NotreDame.txt", 2, 100);

	//benchmark_file("C:/Users/Public/Documents/RWG/soc-Epinions1.txt", 1, 100);

	//benchmark_file("C:/Users/Public/Documents/RHG/16-5-10.txt", 7, 100);
	//benchmark_file("C:/Users/Public/Documents/RHG/16-5-10.txt", 6, 100);
	//benchmark_file("C:/Users/Public/Documents/RHG/16-5-10.txt", 5, 50, 4.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/16-5-10.txt", 4, 50, 4.0 / 6.0);

	//benchmark_file("C:/Users/Public/Documents/RHG/15-5-10.txt", 7, 50, 4.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/15-5-10.txt", 6, 50, 4.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/15-5-10.txt", 7, 50, 5.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/15-5-10.txt", 3, 10, 5.0 / 6.0);
}

/*
 * For comparing LocalEC
 */
void main2() {

	size_t n_L = 5;
	size_t n_S = 10;
	size_t n_R = 10000;
	size_t d = 60;

	size_t N = 10000;

	size_t actual_orig_volume = n_L * (n_L + n_S - 1);
	size_t actual_split_volume = actual_orig_volume + n_L;

	vertex x = 0;
	size_t nu = actual_orig_volume;
	size_t k = n_S + 1;

	enum struct graphtype {
		undirected_fg_lsr,
		sparsified_complete,
		rcLSR,
		RHG,
		RFG
	} const graphtype = graphtype::RFG;

	adj_list adj;
	if constexpr (graphtype == graphtype::sparsified_complete) {
		adj = make_sparsified_complete_LSR_adj_list(n_L, n_S, n_R);
	}
	else if constexpr (graphtype == graphtype::rcLSR) {
		adj = make_randomised_complete_LSR_graph_adj_list(n_L, n_S, n_R);
	}
	else if constexpr (graphtype == graphtype::RHG) {
		x = 876; // in a somewhat small volume cut in this particular graph.
		actual_orig_volume = nu = 646; // This is its volume.
		k = 13;
		ifstream in;
		in.open("C:/Users/Public/Documents/RHG/10-5-10+17.txt");
		adj = read_adjlist_from_edgelist_undirected(in);
		actual_orig_volume = nu = 553; // Sparsified volume.
	}
	else if constexpr (graphtype == graphtype::RFG) {
		adj = versioned_graph::make_undirected_FG_LSR_graph_adj_list(n_L, n_S, n_R, d);
	}

#if false
	auto adj2 = adj;
	for (auto& vec : adj2)
		std::shuffle(vec.begin(), vec.end(), std::default_random_engine());
#elif true
	auto labelling = sparse::nagamochi_ibraki_labelling(adj);
	auto adj2 = sparse::nagamochi_ibraki_adj_list(labelling, k);
	for (auto& vec : adj2)
		std::shuffle(vec.begin(), vec.end(), std::default_random_engine());
#endif

	vertex x_out = 2 * x + 1;

	std::vector<std::pair<localvc::localec_fn*, std::string>> localec1_versions = {
			//{ local_edge_connectivity_v1, "1"},
		//{ local_edge_connectivity_v2, "2"},
			//{ local_edge_connectivity_v3, "3"},
			//{ local_edge_connectivity_v4, "4"},
			//{ local_edge_connectivity_v5, "5"},
		//{ local_edge_connectivity_v8, "8"},
	};
	std::vector<std::pair<localvc2::localec_fn*, std::string>> localec2_versions = {
		{ localvc2::local_edge_connectivity_v2, "2*"},
		{ localvc2::local_edge_connectivity_degreecount, "deg*"},
		{ localvc2::local_edge_connectivity_marking_degreecount, "mdeg*"},
		{ localvc2::local_edge_connectivity_marking2_degreecount, "mdeg2*"},
	};

	auto split = make_split_graph_adj_list(adj2);
	versioned_graph::graph split_g(split);
	resettable_graph::graph split_g2(split);

	bool find_x = true;
	if (find_x) {
		unordered_set<vertex> cut;
		for (vertex v = 0; v < n_L + n_S + n_R; v++) {
			vertex v_out = 2 * v + 1;

			split_g2.reset();
			if (!localvc2::local_edge_connectivity_degreecount(split_g2, v_out, 10 * nu, k, cut))
				continue;

			x = v;
			x_out = v_out;
			std::cout << "x: " << x << std::endl;
			break;
		}
	}

	bool custom_d_i = true;
	bool test_v1 = true;
	bool test_v2 = true;
	bool test2_v2 = false;
	bool test3_v2 = true;

	//vector<double> d_i = { 0.2, 0.5, 1, 2, 3, 4, 6, 8};
	vector<double> d_i = { 1, 2, 3, 4, 6, 8, 10, 12, 14 };
	//vector<double> d_i = { 1, 2, 3, 4, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 36, 40, 48, 56, 64, 72, 80 };

	{
		size_t m = 0;
		for (auto& vec : split_g2) {
			m += vec.size();
		}
		std::cout << "|E(split_g)|: " << m << std::endl;
		std::cout << "base volume: " << nu << std::endl;
	}

	if (test_v1) {
		for (auto& pair : localec1_versions) {
			auto& localec1 = pair.first;
			auto& id = pair.second;
			if (custom_d_i) {
				benchmark_localec_multi_nu(split_g, x_out, nu, k, N, localec1, id, d_i);
			}
			else {
				benchmark_localec_multi_nu(split_g, x_out, nu, k, N, localec1, id);
			}
		}
	}
	if (test_v2) {
		for (auto& pair : localec2_versions) {
			auto& localec2 = pair.first;
			auto& id = pair.second;
			if (custom_d_i) {
				benchmark_localec2_multi_nu(split_g2, x_out, nu, k, N, localec2, id, d_i);
			}
			else {
				benchmark_localec2_multi_nu(split_g2, x_out, nu, k, N, localec2, id);
			}
		}
	}
	if (test2_v2) {
		unordered_set<vertex> cut;

		for (vertex x = 0; x < n_L + n_S + n_R; x++) {
			vertex x_out = 2 * x + 1;

			split_g2.reset();
			if (!localvc2::local_edge_connectivity_degreecount(split_g2, x_out, 10 * nu, k, cut))
				continue;
			std::cout << "x: " << x << std::endl;

			cut.clear();
			for (auto& pair : localec2_versions) {
				auto& localec2 = pair.first;
				auto& id = pair.second;

				benchmark_localec2_multi_nu(split_g2, x_out, nu, k, N, localec2, id, d_i);
			}
		}
	}
	if (test3_v2) {
		vertex x_out = 101; // Probably negative case.

		for (auto& pair : localec2_versions) {
			auto& localec2 = pair.first;
			auto& id = pair.second;
			if (custom_d_i) {
				benchmark_localec2_multi_nu(split_g2, x_out, nu, k, N, localec2, id, d_i);
			}
			else {
				benchmark_localec2_multi_nu(split_g2, x_out, nu, k, N, localec2, id);
			}
		}
	}
}

void main3() {
	ios_base::sync_with_stdio(false);

	size_t N = 20;
	size_t boost = 1;
	double a_coeff = 4.0 / 6.0;

	string fn1 = "C:/Users/Public/Documents/RHG/";
	string fn2 = "-5-10+";
	string fn3 = ".txt";
	for (size_t size_param : {10}) {
		for (size_t i = 0; i < 20; i++) {
			benchmark_file2(fn1 + to_string(size_param) + fn2 + to_string(i) + fn3, N, boost, a_coeff, to_string(i));
		}
	}
}

void main4() {
	size_t n_L = 2;
	size_t n_S = 5;
	size_t n_R = 1000;
	size_t d = 2 * n_S + 3;

	//cout << "Clustered Erdos-Renyi LSR (" << n_L << ", " << n_S << ", " << n_R << ") graph" << endl;
	cout << "Clustered artificial (min degree " << d << ") LSR (" << n_L << ", " << n_S << ", " << n_R << ") graph" << endl;

	double a_coeff = 2.0 / 3.0;
	size_t N = 20;
	size_t boost = 3; // Bypassed currently

	//cout << "p: " << p_in << endl;
	//cout << "np: " << n * p_in << endl;

	//auto adj = make_randomised_complete_LSR_graph_adj_list(n_L, n_S, n_R);
	//auto adj = make_exact_degree_LSR_multigraph_adj_list(n_L, n_S, n_R, 100); // I don't know about this one.
	//auto adj = versioned_graph::make_randomised_undirected_LSR_erdos_renyi(n_L, n_S, n_R, p_in, p_out);
	auto adj = versioned_graph::make_undirected_min_degree_LSR_graph_adj_list(n_L, n_S, n_R, d);

	localec_fn* localec = local_edge_connectivity_v2;
	globalvc_ud_fn* unbalancedvc = vertex_connectivity_unbalanced_undirected_edge_sampling;
	//globalvc_ud_fn* unbalancedvc2 = vertex_connectivity_unbalanced_undirected_edge_sampling_to_a;
	globalvc_ud_fn* unbalancedvc3 = vertex_connectivity_unbalanced_undirected_vertex_sampling;
	globalvc_balanced_fn* balancedvc = vertex_connectivity_push_relabel_edge_sampling;
	globalvc_balanced_fn* balancedvc2 = vertex_connectivity_balanced_edge_sampling;
	globalvc_balanced_fn* balancedvc3 = vertex_connectivity_balanced_edge_sampling_adj_list;
	globalvc_balanced_fn* balancedvc4 = vertex_connectivity_balanced_vertex_sampling_adj_list;
	henzinger::henzinger_vc_fn* henzingervc = henzinger::vc_undirected_linear_unsparsified_random;

	benchmark_henzinger(henzingervc, adj, N);
	benchmark_find_VC_undirected(VC_undirected_edge_sampling, localec, unbalancedvc, balancedvc3, adj, a_coeff, 0, N, "edge");
	benchmark_find_VC_undirected(VC_undirected_edge_sampling, localec, unbalancedvc, balancedvc3, adj, a_coeff, 1, N, "edge");
	benchmark_find_VC_undirected(VC_undirected_edge_sampling, localec, unbalancedvc, balancedvc3, adj, a_coeff, 2, N, "edge");

	benchmark_find_VC_undirected(VC_undirected_vertex_sampling, localec, unbalancedvc3, balancedvc4, adj, a_coeff, 0, N, "vertex");
	benchmark_find_VC_undirected(VC_undirected_vertex_sampling, localec, unbalancedvc3, balancedvc4, adj, a_coeff, 1, N, "vertex");
	benchmark_find_VC_undirected(VC_undirected_vertex_sampling, localec, unbalancedvc3, balancedvc4, adj, a_coeff, 2, N, "vertex");
}

void main8() {
	bool verbose = true;
	bool use_binary = false;
	bool via_graph = false;

	string path = "C:/Users/Public/Documents/RWG/";
	string suffix = (use_binary ? ".bin" : ".txt");

	size_t k = 200;

	string name1 = "soc-Epinions1";
	string name14 = "com-friendster.ungraph";
	string name = name14;

	string name_in = name;
	string name_out = name + "-inplace-k-" + to_string(k);
	name_in = name_out; // For iterating

	size_t skip_lines = 4;

	//string path_in = path + name_in + suffix;
	string path_out = path + name_out + suffix;
	string path_tmp = path + "tmp.tmp";

	std::cout << "Path: " << path_out << std::endl;
	std::cout << "Temp: " << path_tmp << std::endl;

	for (size_t i = 0; i < 100; i++) {
		if (verbose) std::cout << "Starting iteration " << i << std::endl;

		std::vector<size_t> degrees;
		{
			if (verbose) std::cout << "Reading degrees... ";
			if (use_binary)
				degrees = read_degrees_from_binary_undirected(path_out);
			else {
				ifstream in;
				in.open(path_out);
				degrees = read_degrees_from_edgelist_undirected(in, skip_lines);
			}

			if (verbose) std::cout << "done reading degrees " << std::endl;
		}
		size_t n = degrees.size();

		if (verbose) std::cout << "Constructing subgraph mask... ";
		size_t projected_m = 0;
		std::vector<bool> subgraph_mask;
		bool exists_lowdegree = false;
		subgraph_mask.reserve(n);
		for (size_t d : degrees) {
			subgraph_mask.push_back(d >= k);
			exists_lowdegree = exists_lowdegree || (d != 0 && d < k);
			if (d >= k)
				projected_m += d;
		}
		if (verbose) std::cout << "done constructing subgraph mask " << std::endl;
		std::cout << "Projected m for next iteration: " << projected_m / 2 << " * 2" << std::endl;

		adj_list G;
		if (via_graph) {
			if (verbose) std::cout << "Reconstructing subgraph... " << std::endl;
			if (use_binary)
				G = read_subgraph_from_binary_undirected(path_out, subgraph_mask);
			else {
				ifstream in;
				in.open(path_out);
				G = read_subgraph_from_edgelist_undirected(in, subgraph_mask, skip_lines);
			}
			if (verbose) std::cout << "done reconstructing subgraph " << std::endl;
			if (verbose) std::cout << "Processing LCC... " << std::endl;
			preprocessing::inplace_remove_duplicate_edges(G);
			preprocessing::in_place_LWCC(G);
			if (verbose) std::cout << "done processing LCC " << std::endl;
		}
		if (!via_graph) {
			if (verbose) std::cout << "Reconstructing subgraph... " << std::endl;
			if (use_binary)
				read_and_write_subgraph_binary_undirected(path_out, path_tmp, subgraph_mask);
			else
				read_and_write_subgraph_edgelist_undirected(path_out, path_tmp, subgraph_mask, skip_lines);

			if (verbose) std::cout << "done reconstructing subgraph " << std::endl;
		}

		if (false) {
			size_t n_out = G.size();
			size_t m_out = 0;
			for (auto& vec : G) {
				m_out += vec.size();
			}
			std::cout << "n: " << n_out << "\t";
			std::cout << "m: " << m_out / 2 << "*2" << std::endl;
		}

		if (false) {
			ofstream out;
			out.open(path_tmp);
			write_adjlist_to_edgelist_undirected(out, G);
		}

		std::swap(path_out, path_tmp);

		if (!exists_lowdegree)
			break;
	}



}

void main9() {
	string path = "C:/Users/Public/Documents/RWG/";

	string name1 = "soc-Epinions1";
	string name2 = "soc-Epinions2";
	string name14 = "com-friendster.ungraph";

	string name = name14;
	size_t skip = 4;

	string path_in = path + name + "-inplace-k-200" + ".txt";
	string path_out = path + name + "-inplace-k-200" + ".bin";

	versioned_graph::adj_list G1, G2;

	std::cout << "A" << std::endl;

	bool use_bin = true;
	bool via_graph = false;

	if (via_graph) {
		if (use_bin) {
			ifstream in;
			in.open(path_in);
			G1 = read_adjlist_from_edgelist_undirected(in, skip);
			write_adjlist_to_binary_undirected(path_out, G1);
		}
		if (!use_bin) {
			ifstream in;
			in.open(path_in);
			G1 = read_adjlist_from_edgelist_undirected(in, skip);
			ofstream out;
			out.open(path_out);
			write_adjlist_to_edgelist_undirected(out, G1);
		}
	}

	std::cout << "B" << std::endl;

	if (!via_graph) {
		if (!use_bin) return; // error

		std::cout << "path in:  " << path_in << std::endl;
		std::cout << "path out: " << path_out << std::endl;
		read_edgelist_write_binary(path_in, path_out, skip);
	}

	std::cout << "C" << std::endl;

	if (via_graph) {
		if (use_bin)
			G2 = read_adjlist_from_binary_undirected(path_out);
		if (!use_bin) {
			ifstream in;
			in.open(path_out);
			G2 = read_adjlist_from_edgelist_undirected(in);
		}


		size_t n1 = G1.size();
		size_t n2 = G2.size();

		std::cout << "G1.size: " << n1 << std::endl;
		std::cout << "G2.size: " << n2 << std::endl;

		if (n1 == n2) {
			for (size_t s = 0; s < n1; s++) {
				size_t d = G1[s].size();
				if (d != G2[s].size()) {
					std::cout << "Graphs are different." << std::endl;
					std::cout << "Degree of vertex v turns from " << d << " to " << G2[s].size() << std::endl;
					return;
				}
				for (size_t i = 0; i < d; i++) {
					if (G1[s][i] != G2[s][i]) {
						std::cout << "Graphs are different." << std::endl;

						std::cout << "G1[" << s << "] =";
						for (size_t v : G1[s])
							std::cout << " " << v;
						std::cout << std::endl;

						std::cout << "G2[" << s << "] =";
						for (size_t v : G2[s])
							std::cout << " " << v;
						std::cout << std::endl;

						return;
					}
				}
			}
		}
		std::cout << "Graphs match." << std::endl;
	}


}