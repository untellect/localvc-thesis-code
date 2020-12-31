#include "pch.h"

#include <iostream>
#include <fstream>
#include <chrono>
//#include <math.h>

#include "GraphIO.h"
#include "localvc.h"
#include "Util.h"

#include "Benchmark.h"

using namespace localvc;
using namespace std;
using namespace std::chrono;

#define VERBOSE_PRINTING
#define PRINTCUTVOLUME

void benchmark_file2(string filename, size_t k, size_t N = 100, double a_coeff = 2.0 / 3.0) {
	ifstream in;
	in.tie(NULL);
	in.open(filename);

	auto t_0 = high_resolution_clock::now();
	
	auto adj = read_adjlist_from_edgelist_undirected(in);
	graph g = graph(adj);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	cout << "It took " << dt.count() << " ms to read the file." << endl;
	//cout << g;
	cout << "G has " << g.size_vertices() << " vertices and " << g.size_edges() << " edges." << endl;
	cout << "k: " << k << endl;

	size_t a2 = pow(g.size_edges(), a_coeff);

	//benchmark_globalvc(g, k, a2, N, vertex_connectivity_unbalanced_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced mixed 2 hybrid 2");
	benchmark_ud3(g, k, a2, N, vertex_connectivity_unbalanced_undirected_mixed2_hybrid_sampling, local_edge_connectivity_v2, "unbalanced undirected mixed 2 hybrid 2");
}

void main() {
	ios_base::sync_with_stdio(false);

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
	//benchmark_file("C:/Users/Public/Documents/RHG/15-5-10.txt", 6, 50, 5.0 / 6.0);

	//benchmark_file("C:/Users/Public/Documents/RHG/17-5-10.txt", 5, 50, 4.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/17-5-10.txt", 4, 50, 4.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/17-5-10.txt", 5, 20, 5.0 / 6.0);
	//benchmark_file("C:/Users/Public/Documents/RHG/17-5-10.txt", 4, 20, 5.0 / 6.0);

	benchmark_file("C:/Users/Public/Documents/RHG/10-5-10.txt", 7, 50, 4.0 / 6.0);
	benchmark_file("C:/Users/Public/Documents/RHG/10-5-10.txt", 6, 50, 4.0 / 6.0);
	benchmark_file("C:/Users/Public/Documents/RHG/10-5-10.txt", 7, 50, 5.0 / 6.0);
	benchmark_file("C:/Users/Public/Documents/RHG/10-5-10.txt", 6, 50, 5.0 / 6.0);
}
