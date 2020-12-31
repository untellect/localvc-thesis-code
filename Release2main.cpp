#include "pch.h"

//#define VERBOSE_PRINTING

#include <iostream>
#include <chrono>
#include <math.h>

#include "localvc.h"
#include "Graph.h"
#include "Graphgen.h"
#include "Util.h"

#include "Benchmark.h"

using namespace localvc;
using namespace std;
using namespace std::chrono;


void benchmark(graph& g, size_t k, size_t a, size_t N, globalvc_fn globalvc, localec_fn localec, string id) { // local alias because it's used so many times.
	benchmark_globalvc(g, k, a, N, globalvc, localec, id);
}

/*
void benchmark_ud_b(graph& g, size_t k, size_t a, size_t N, globalvc_balanced_fn globalvc, localec_fn localec, string id) {
	size_t successes = 0;
	cout << "Starting " << N << " iterations of globalvc " << id << endl;
	auto t_0 = high_resolution_clock::now();
	graph g_split = make_split_graph(g);
	for (size_t i = 0; i < N; ++i) {
		vertex x;
		auto cutpair = globalvc(g, g_split, k, a, localec, x);
		if (cutpair.second == false) { // cutpair.second == false iff we found a cut.

			//cout << "Cut size: " << cutpair.first.size() << endl;
			//for (vertex v : cutpair.first) std::cout << v << " ";
			//std::cout << std::endl;

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

void main() {

	// {n_L, n_S, n_R, d}
	vector<vector<size_t>> LSR = {
		{100,  5, 1000,   100, 6},
		{100,  5, 2000,   100, 6},
		{100,  5, 4000,   100, 6},
		{100,  5, 7000,   100, 6},
		{100,  5, 10000,  100, 6},
		{100,  5, 20000,  100, 6},
		{100,  5, 40000,  100, 6},
		{100,  5, 70000,  100, 6},
		{100,  5, 100000, 100, 6},

		//{1,   5, 1400 - 1,   100, 6},
		//{10,  5, 1400 - 10,  100, 6},
		//{100, 5, 1400 - 100, 100, 6},
		//{400, 5, 1400 - 400, 100, 6},

		//{100, 5, 1000, 50,  6},
		//{100, 5, 1000, 100, 6},
		//{100, 5, 1000, 200, 6},

		//{50, 5, 1000, 100, 5},
		//{50, 5, 1000, 500, 6},

		//{50, 5, 1000, 100, 6},
		//{25, 5, 1000, 10, 6},

		//{100,  2, 1000,   100, 3},
		//{100,  2, 2000,   100, 3},
		//{100,  2, 4000,   100, 3},
		//{100,  2, 7000,   100, 3},
		//{100,  2, 10000,  100, 3},
		//{100,  2, 20000,  100, 3},
		//{100,  2, 40000,  100, 3},
	};

	for (vector<size_t>& lsr : LSR) {
		size_t n_L = lsr[0];
		size_t n_S = lsr[1];
		size_t n_R = lsr[2];
		size_t d   = lsr[3];
		size_t k   = lsr[4];

		std::chrono::time_point<std::chrono::high_resolution_clock> t_0, t_1;
		std::chrono::milliseconds dt;

		cout << "Making sparsified complete LSR graph for benchmarking." << endl;
		//cout << "Making complete LSR graph for benchmarking." << endl;
		cout << "(L,S,R) = (" << n_L << "," << n_S << "," << n_R << ")" << endl;
		//cout << "Making exact degree LSR multigraph for benchmarking." << endl;
		//cout << "(L,S,R,d) = (" << n_L << "," << n_S << "," << n_R << "," << d << ")" << endl;
		t_0 = chrono::high_resolution_clock::now();
		graph g = make_sparsified_complete_LSR_graph(n_L, n_S, n_R);
		//graph g = make_exact_degree_LSR_multigraph(n_L, n_S, n_R, d);
		t_1 = chrono::high_resolution_clock::now();
		dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
		cout << "Making graph took: " << dt.count() << " ms." << endl;

		cout << "n: " << g.size_vertices() << endl;
		cout << "m: " << g.size_edges() << endl;

//#define SPARSIFY

//#define COMPARE_SAMPLING
//#define COMPARE_EC
//#define SMALL_HYBRID_2
//#define MIXED2_HYBRID_2
#define MIXED2_HYBRID_2_UD
//#define MIXED_HYBRID_2
//#define MIXED_HYBRID_7
//#define SPARSIFIED_MIXED_SKIPLOW_2
//#define HYBRID_2
//#define HYBRID_LP_OPTIMISED_2

#define YES_CASE
#define NOT_CASE

#ifdef COMPARE_SAMPLING
	#define MIXED_HYBRID_2
	//#define HYBRID_2
	#define MIXED_VERTEX_2
	//#define VERTEX_2
	#define MIXED_SKIPLOW_2
	#define SPARSIFIED_MIXED_VERTEX_2
	#define SPARSIFIED_MIXED_SKIPLOW_2
#endif

#ifdef COMPARE_EC
	#define MIXED_HYBRID_1
	#define MIXED_HYBRID_2
	#define MIXED_HYBRID_3
	#define MIXED_HYBRID_4
	#define MIXED_HYBRID_5
	#define MIXED_HYBRID_6
	#define MIXED_HYBRID_7
#endif

#ifdef SPARSIFY
		cout << "Making sparse version of graph." << endl;
		t_0 = chrono::high_resolution_clock::now();
		//graph g = make_complete_LSR_graph(n_L, n_S, n_R);
		g = make_sparse_graph(g, k);
		t_1 = chrono::high_resolution_clock::now();
		dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
		cout << "Making sparse graph took: " << dt.count() << " ms." << endl;
#endif

		size_t N = 1000;
		size_t a = sqrt(g.size_edges() / k);
		size_t a2 = pow(g.size_edges(), 2.0 / 3.0);

#ifdef YES_CASE
	#ifdef MIXED_HYBRID_1
			cout << "k = " << k << ", a = " << a2 << endl;
			benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v1, "unbalanced mixed hybrid 1");
	#endif

	#ifdef SMALL_HYBRID_2
			cout << "k = " << k << ", a = " << a2 << endl;
			benchmark(g, k, a2, N, vertex_connectivity_unbalanced_small_hybrid_sampling, local_edge_connectivity_v2, "unbalanced small hybrid 2");
	#endif
	#ifdef MIXED2_HYBRID_2
			cout << "k = " << k << ", a = " << a2 << endl;
			benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2");
	#endif
	#ifdef MIXED2_HYBRID_2_UD
			cout << "k = " << k << ", a = " << a2 << endl;
			benchmark_ud3(g, k, a2, N, vertex_connectivity_unbalanced_undirected_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2 ud");
	#endif
	#ifdef MIXED_HYBRID_2
			cout << "k = " << k << ", a = " << a2 << endl;
			benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed hybrid 2");
	#endif
	#ifdef HYBRID_2
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_hybrid_sampling, local_edge_connectivity_v2, "unbalanced hybrid 2");

	#endif
	#ifdef MIXED_VERTEX_2
		cout << "k = " << k << ", a = " << a << endl;
		benchmark(g, k, a, N, vertex_connectivity_unbalanced_mixed_vertex_sampling, local_edge_connectivity_v2, "unbalanced mixed vertex 2");
	#endif
	#ifdef VERTEX_2
		cout << "k = " << k << ", a = " << a << endl;
		benchmark(g, k, a, N, vertex_connectivity_unbalanced_vertex_sampling, local_edge_connectivity_v2, "unbalanced vertex 2");
	#endif
	#ifdef SPARSIFIED_MIXED_VERTEX_2
		cout << "k = " << k << ", a = " << a << endl;
		benchmark(g, k, a, N, vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling, local_edge_connectivity_v2, "unbalanced mixed vertex sparsified 2");
	#endif
	#ifdef MIXED_SKIPLOW_2
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling_skiplow, local_edge_connectivity_v2, "unbalanced mixed skip low 2");
	#endif
	#ifdef SPARSIFIED_MIXED_SKIPLOW_2
		cout << "k = " << k << ", a = " << a << endl;
		benchmark(g, k, a, N, vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling_skiplow, local_edge_connectivity_v2, "unbalanced mixed sparsified skip low 2");
	#endif
	#ifdef HYBRID_LP_OPTIMISED_2
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_hybrid_lp, local_edge_connectivity_v2, "unbalanced hybrid linear optimised 2");
	#endif
		

	#ifdef MIXED_HYBRID_3
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v3, "unbalanced mixed hybrid 3");
	#endif
	#ifdef MIXED_HYBRID_4
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v4, "unbalanced mixed hybrid 4");
	#endif
	#ifdef MIXED_HYBRID_5
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v5, "unbalanced mixed hybrid 5");
	#endif
	#ifdef MIXED_HYBRID_6
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v6, "unbalanced mixed hybrid 6");
	#endif
	#ifdef MIXED_HYBRID_7
		cout << "k = " << k << ", a = " << a2 << endl;
		benchmark(g, k, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v7, "unbalanced mixed hybrid 7");
	#endif
#endif // YES_CASE

#ifdef NOT_CASE
	#ifdef MIXED_HYBRID_1
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v1, "unbalanced mixed hybrid 1");
	#endif

	#ifdef SMALL_HYBRID_2
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_small_hybrid_sampling, local_edge_connectivity_v2, "unbalanced small hybrid 2");
	#endif
	#ifdef MIXED2_HYBRID_2
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2");
	#endif
	#ifdef MIXED2_HYBRID_2_UD
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark_ud3(g, k - 1, a2, N, vertex_connectivity_unbalanced_undirected_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2 ud");
	#endif
	#ifdef MIXED_HYBRID_2
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed hybrid 2");
	#endif
	#ifdef HYBRID_2
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_hybrid_sampling, local_edge_connectivity_v2, "unbalanced hybrid 2");
	#endif
	#ifdef MIXED_VERTEX_2
			cout << "k = " << k - 1 << ", a = " << a << endl;
			benchmark(g, k - 1, a, N, vertex_connectivity_unbalanced_mixed_vertex_sampling, local_edge_connectivity_v2, "unbalanced mixed vertex 2");
	#endif
	#ifdef VERTEX_2
			cout << "k = " << k - 1 << ", a = " << a << endl;
			benchmark(g, k - 1, a, N, vertex_connectivity_unbalanced_vertex_sampling, local_edge_connectivity_v2, "unbalanced vertex 2");
	#endif
	#ifdef SPARSIFIED_MIXED_VERTEX_2
			cout << "k = " << k - 1 << ", a = " << a << endl;
			benchmark(g, k - 1, a, N, vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling, local_edge_connectivity_v2, "unbalanced mixed vertex sparsified 2");
	#endif
	#ifdef MIXED_SKIPLOW_2
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling_skiplow, local_edge_connectivity_v2, "unbalanced mixed skip low 2");
	#endif
	#ifdef SPARSIFIED_MIXED_SKIPLOW_2
			cout << "k = " << k - 1 << ", a = " << a << endl;
			benchmark(g, k - 1, a, N, vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling_skiplow, local_edge_connectivity_v2, "unbalanced mixed sparsified skip low 2");
	#endif
	#ifdef HYBRID_LP_OPTIMISED_2
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_hybrid_lp, local_edge_connectivity_v2, "unbalanced hybrid linear optimised 2");
	#endif

	#ifdef MIXED_HYBRID_3
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v3, "unbalanced mixed hybrid 3");
	#endif
	#ifdef MIXED_HYBRID_4
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v4, "unbalanced mixed hybrid 4");
	#endif
	#ifdef MIXED_HYBRID_5
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v5, "unbalanced mixed hybrid 5");
	#endif
	#ifdef MIXED_HYBRID_6
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v6, "unbalanced mixed hybrid 6");
	#endif
	#ifdef MIXED_HYBRID_7
			cout << "k = " << k - 1 << ", a = " << a2 << endl;
			benchmark(g, k - 1, a2, N, vertex_connectivity_unbalanced_mixed_hybrid_sampling, local_edge_connectivity_v7, "unbalanced mixed hybrid 7");
	#endif
#endif // NOT_CASE

	}
}