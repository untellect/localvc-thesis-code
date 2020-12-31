#include "pch.h"
#include "Localvc.h"
#include "Graphgen.h"
#include "MaxFlow.h"

using namespace std;
using namespace versioned_graph;
using namespace localvc;
using namespace maxflow;

TEST(balanced_vertex_connectivity, canfindcut) {
	size_t l_size = 50;
	size_t s_size = 5;
	size_t r_size = 50;
	size_t k = s_size + 1;
	adj_list g = make_randomised_complete_LSR_graph_adj_list(l_size, s_size, r_size);
	size_t a = g.size() / 10;
	
	bool found_cut;
	unordered_set<vertex> cut;

	found_cut = vertex_connectivity_balanced_vertex_sampling(g, k, a, cut);
	for (int i = 0; i < 10 && !found_cut; ++i) { // A few repeats if it fails.
		found_cut = vertex_connectivity_balanced_vertex_sampling(g, k, a, cut);
	}

	bool kconnected = !found_cut;
	ASSERT_FALSE(kconnected); // Should be unlikely to fail with the repeats
	vector<vertex> C(cut.begin(), cut.end());
	sort(C.begin(), C.end());
	ASSERT_EQ(C.size(), 5);
	EXPECT_EQ(C[0], 50);
	EXPECT_EQ(C[1], 51);
	EXPECT_EQ(C[2], 52);
	EXPECT_EQ(C[3], 53);
	EXPECT_EQ(C[4], 54);
}

TEST(balanced_vertex_connectivity, wontfindfalsecut) {
	size_t l_size = 50;
	size_t s_size = 5;
	size_t r_size = 50;
	size_t k = s_size;
	adj_list g = make_randomised_complete_LSR_graph_adj_list(l_size, s_size, r_size);
	size_t a = g.size() / 10;

	bool found_cut;
	unordered_set<vertex> cut;

	found_cut = vertex_connectivity_balanced_vertex_sampling(g, k, a, cut);
	for (int i = 0; i < 10 && !found_cut; ++i) { // A few repeats if it fails.
		found_cut = vertex_connectivity_balanced_vertex_sampling(g, k, a, cut);
	}

	bool kconnected = !found_cut;
	vector<vertex> C(cut.begin(), cut.end());
	sort(C.begin(), C.end());
	EXPECT_TRUE(kconnected); // Can't ever be false if everything works correctly.
	ASSERT_EQ(C.size(), 0);
}
