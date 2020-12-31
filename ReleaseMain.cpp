#include "pch.h"

#include "Localec-templated.h"
#include "localvc.h"
#include "Graph.h"
#include "Graphgen.h"

#include <iostream>
#include <chrono>
#include <math.h>

#include "Benchmark.h"

using namespace std;
using namespace std::chrono;
using namespace localvc;

void main() {
	size_t n_L = 2;
	size_t n_S = 5;
	size_t n_R = 1000;

	std::chrono::time_point<std::chrono::high_resolution_clock> t_0, t_1;
	std::chrono::milliseconds dt;

#define G1_ON
#ifdef G1_ON
	cout << "Making LSR graph for benchmarking." << endl;
	t_0 = chrono::high_resolution_clock::now();
	graph g = make_complete_LSR_graph(n_L, n_S, n_R);
	t_1 = chrono::high_resolution_clock::now();
	dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "Making LSR graph took: " << dt.count() << " ms." << endl;

	cout << "Now making split graph." << endl;
	t_0 = chrono::high_resolution_clock::now();
	g = make_split_graph(g);
	t_1 = chrono::high_resolution_clock::now();
	dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "Making split graph took: " << dt.count() << " ms." << endl;
#endif

	vertex x = 1; // Out-vertex of 0 (2*0 + 1)
	vertex y = 21; // Out-vertex of 10 (2*10 +1) which is in R.
	size_t mu = n_L * (n_L + n_S - 1);
	size_t accurate_split_mu = mu + n_L + n_S;
	size_t split_mu = 3 * mu; // Probably wasteful but it's from the article.

	size_t k = n_S + 1;

	size_t N = 10000;

#ifdef G1_ON
#define ON_1
//#define ON_2
//#define ON_3
//#define ON_4
//#define ON_5
#endif

#ifdef ON_1
	cout << "Accuracy benchmark on a split graph of a complete LSR graph with (|L|,|S|,|R|)=(" << n_L << "," << n_S << "," << n_R << "), k=" << k << " and mu=" << split_mu << endl;
	cout << "Mu uses the 3x approximation" << endl;

	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v1, "1");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v2, "2");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v3, "3");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v4, "4");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v5, "5");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v6, "6");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_v7, "7");
	benchmark_localec(g, x, split_mu, k, N, local_edge_connectivity_templated, "T");

#endif
#ifdef ON_2
	cout << "Accuracy benchmark on a split graph of a complete LSR graph with (|L|,|S|,|R|)=(" << n_L << "," << n_S << "," << n_R << "), k=" << k << " and mu=" << accurate_split_mu << endl;
	cout << "Mu is the number of outgoing edges in L' (including invertices in S)" << endl;

	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v1, "1");
	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v2, "2");
	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v3, "3");
	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v4, "4");
	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v5, "5");
	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v6, "6");
	benchmark_localec(g, x, accurate_split_mu, k, N, local_edge_connectivity_v7, "7");
#endif
#ifdef ON_3
	cout << "Making randomised version" << endl;

	graph g_r = make_randomised_complete_LSR_graph(n_L, n_S, n_R);
	graph split_g_r = make_split_graph(g);

	cout << "Accuracy benchmark on a split graph of a randomised complete LSR graph with (|L|,|S|,|R|)=(" << n_L << "," << n_S << "," << n_R << "), k=" << k << " and mu=" << split_mu << endl;
	cout << "Mu uses the 3x approximation" << endl;

	benchmark_localec(split_g_r, x, split_mu, k, N, local_edge_connectivity_v1, 1);
	benchmark_localec(split_g_r, x, split_mu, k, N, local_edge_connectivity_v2, 2);
	//benchmark_localec(split_g_r, x, split_mu, k, N, local_edge_connectivity_v3, 3);
	//benchmark_localec(split_g_r, x, split_mu, k, N, local_edge_connectivity_v4, 4);
	//benchmark_localec(split_g_r, x, split_mu, k, N, local_edge_connectivity_v5, 5);
	//benchmark_localec(split_g_r, x, split_mu, k, N, local_edge_connectivity_v6, 6);
#endif
#ifdef ON_4
	cout << "Fail speed benchmark on a split graph of a complete LSR graph with (|L|,|S|,|R|)=(" << n_L << "," << n_S << "," << n_R << "), k=" << k << " and mu=" << split_mu << endl;
	cout << "Mu uses the 3x approximation" << endl;

	benchmark_localec(split_g, y, split_mu, k, N, local_edge_connectivity_v1, 1);
	benchmark_localec(split_g, y, split_mu, k, N, local_edge_connectivity_v2, 2);
	//benchmark_localec(split_g, y, split_mu, k, N, local_edge_connectivity_v3, 3);
	//benchmark_localec(split_g, y, split_mu, k, N, local_edge_connectivity_v4, 4);
	//benchmark_localec(split_g, y, split_mu, k, N, local_edge_connectivity_v5, 5);
	//benchmark_localec(split_g, y, split_mu, k, N, local_edge_connectivity_v6, 6);
#endif

#ifdef ON_5
	benchmark_localec(g, x, mu / 2, k, N, local_edge_connectivity_v7, ".5 mu");
	benchmark_localec(g, x, mu, k, N, local_edge_connectivity_v7, "1 mu");
	benchmark_localec(g, x, 2 * mu, k, N, local_edge_connectivity_v7, "2 mu");
	benchmark_localec(g, x, 4 * mu, k, N, local_edge_connectivity_v7, "4 mu");
	benchmark_localec(g, x, 8 * mu, k, N, local_edge_connectivity_v7, "8 mu");
	benchmark_localec(g, x, 16 * mu, k, N, local_edge_connectivity_v7, "16 mu");
	benchmark_localec(g, x, 32 * mu, k, N, local_edge_connectivity_v7, "32 mu");
	benchmark_localec(g, x, 64 * mu, k, N, local_edge_connectivity_v7, "64 mu");
	benchmark_localec(g, x, 128 * mu, k, N, local_edge_connectivity_v7, "128 mu");
#endif
}