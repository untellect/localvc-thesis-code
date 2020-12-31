#include "pch.h"
#include "Localvc.h"
#include "Graphgen.h"

using namespace std;
using namespace versioned_graph;
using namespace localvc;

TEST(GLOBALVC_TEST_NAME, canfindcut) {
	size_t l_size = 6;
	size_t s_size = 5;
	size_t r_size = 50;
	size_t k = s_size + 1;
	graph g = make_randomised_complete_LSR_graph(l_size, s_size, r_size);
	size_t a = g.size_vertices(); // Issue: same value regardless of sampling mode.
	
	globalvc_graphs g2(g);
	auto cutpair = GLOBALVC_FUNC(g2, k, a, LOCALEC_FUNC);
	for (int i = 0; i < 10 && cutpair.second; ++i) { // A few repeats if it fails.
		cutpair = GLOBALVC_FUNC(g2, k, a, LOCALEC_FUNC);
	}

	unordered_set<vertex>& cut = cutpair.first;
	bool kconnected = cutpair.second;
	EXPECT_FALSE(kconnected); // Should be unlikely to fail with the repeats
	vector<vertex> L(cutpair.first.begin(), cutpair.first.end());
	EXPECT_EQ(L.size(), s_size);
	sort(L.begin(), L.end());

	stringstream ss;
	for (vertex v : L)
		ss << v << " ";
	string expected = "6 7 8 9 10 ";
	EXPECT_EQ(ss.str(), expected);
}

TEST(GLOBALVC_TEST_NAME, wontfindcut) {
	size_t l_size = 10;
	size_t s_size = 5;
	size_t r_size = 50;
	size_t k = s_size;
	graph g = make_randomised_complete_LSR_graph(l_size, s_size, r_size);
	size_t a = g.size_vertices() / 10;

	globalvc_graphs g2(g);
	auto cutpair = GLOBALVC_FUNC(g2, k, a, LOCALEC_FUNC);

	unordered_set<vertex>& cut = cutpair.first;
	bool kconnected = cutpair.second;
	EXPECT_TRUE(kconnected);
}