#include "pch.h"

#include <iostream>
#include "Benchmark.h"

using namespace std;

void localec2_test() {
	size_t n_L = 20;
	size_t n_S = 10;
	size_t n_R = 4000;

	size_t actual_orig_volume = n_L * (n_L + n_S - 1);
	size_t actual_split_volume = actual_orig_volume + n_L;

	vertex x = 0;
	size_t nu = actual_orig_volume;
	size_t k = n_S + 1;


	bool using_RHG = false;

#if true
	auto adj = make_sparsified_complete_LSR_adj_list(n_L, n_S, n_R);
#elif !using_RHG
	auto adj = make_randomised_complete_LSR_graph_adj_list(n_L, n_S, n_R);
#elif using_RHG
	x = 876; // in a somewhat small volume cut in this particular graph.
	actual_orig_volume = nu = 646; // This is its volume.
	k = 13;
	ifstream in;
	in.open("C:/Users/Public/Documents/RHG/10-5-10+17.txt");
	auto adj = read_adjlist_from_edgelist_undirected(in);
	actual_orig_volume = nu = 553; // Sparsified volume.
#endif

#if false
	auto adj2 = adj;
	for (auto& vec : adj2)
		std::shuffle(vec.begin(), vec.end(), std::default_random_engine());
#else
	auto labelling = sparse::nagamochi_ibraki_labelling(adj);
	auto adj2 = sparse::nagamochi_ibraki_adj_list(labelling, k);
	for (auto& vec : adj2)
		std::shuffle(vec.begin(), vec.end(), std::default_random_engine());
#endif

	vertex x_out = 2 * x + 1;

	std::vector<std::pair<localvc::localec_fn*, std::string>> localec1_versions = {
		//{ local_edge_connectivity_v1, "1"},
		{ local_edge_connectivity_v2, "2"},
		//{ local_edge_connectivity_v3, "3"},
		//{ local_edge_connectivity_v4, "4"},
		//{ local_edge_connectivity_v5, "5"},
		//{ local_edge_connectivity_v6, "6"},
		//{ local_edge_connectivity_v7, "7"},
	};
	std::vector<std::pair<localvc2::localec_fn*, std::string>> localec2_versions = {
		{ localvc2::local_edge_connectivity_v2, "2*"},
		{ localvc2::local_edge_connectivity_degreecount, "deg*"},
	};

	auto split = make_split_graph_adj_list(adj2);
	versioned_graph::graph split_g(split);
	resettable_graph::graph split_g2(split);

	bool custom_d_i = true;
	bool test_v1 = true;
	bool test_v2 = true;

	size_t N = 1;

	vector<double> d_i = { 1, 2, 3, 4, 6, 8, 10, 12, 14 };
	//vector<double> d_i = { 1, 2, 3, 4, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 36, 40, 48, 56, 64, 72, 80 };

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
}

void RFGgen() {
	high_resolution_clock::time_point t1, t2;

	std::cout << "n\tt" << std::endl;
	for (size_t n = 1000; n < 20000; n += 1000) {
		size_t L = 5;
		size_t S = 8;
		size_t R = n - L - S;
		size_t k = 60;

		t1 = high_resolution_clock::now();
		make_undirected_FG_LSR_graph_adj_list(L, S, R, k);
		t2 = high_resolution_clock::now();

		size_t t = duration_cast<milliseconds>(t2 - t1).count();

		std::cout << n << "\t" << t << std::endl;
	}
}

void main() {
	RFGgen();
}