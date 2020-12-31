#include "pch.h"
#include "Graphgen.h"
#include <vector>
#include "MaxFlow.h"

using namespace std;
using namespace versioned_graph;

TEST(Graphgen, CompleteGraph) {
	size_t n = 10;
	graph g = make_complete_graph(n);

	EXPECT_EQ(g.size_vertices(), 10);
	EXPECT_EQ(g.size_edges(), 10 * 9);
	for (vertex s = 0; s < 10; ++s) {
		auto eit = g.begin_of(s);
		for (vertex t = 0; t < 10; ++t) {
			if (s == t)
				continue;
			EXPECT_EQ(s, eit.get_source());
			EXPECT_EQ(t, eit.get_target());
			++eit;
		}
	}
}

TEST(Graphgen, RandomisedCompleteGraph) {
	size_t n = 10;
	graph g = make_randomised_complete_graph(n);

	EXPECT_EQ(g.size_vertices(), 10);
	EXPECT_EQ(g.size_edges(), 10 * 9);
	bool ordered = true;
	for (vertex s = 0; s < 10 && ordered; ++s) {
		auto eit = g.begin_of(s);
		for (vertex t = 0; t < 10 && ordered; ++t) {
			if (s == t)
				continue;
			if (t != eit.get_target())
				ordered = false;
			++eit;
		}
	}
	EXPECT_FALSE(ordered);
}

TEST(Graphgen, CompleteLSRGraph) {
	size_t l = 5;
	size_t s = 2;
	size_t r = 10;
	graph g = make_complete_LSR_graph(l, s, r);

	EXPECT_EQ(g.size_vertices(), l + s + r);
	EXPECT_EQ(g.size_edges(), l * (l + s - 1) + s * (l + s + r - 1) + r * (s + r - 1));

	auto end = g.end_of(2);
	size_t n = 0;
	for (auto eit = g.begin_of(2); eit != end; ++eit) {
		EXPECT_LT(eit.get_target(), l + s);
		if (n == 2)
			++n;
		EXPECT_EQ(eit.get_target(), n);
		++n;
	}
	EXPECT_EQ(n, l + s);

	end = g.end_of(6);
	n = 0;
	for (auto eit = g.begin_of(6); eit != end; ++eit) {
		if (n == 6)
			++n;
		EXPECT_EQ(eit.get_target(), n);
		++n;
	}
	EXPECT_EQ(n, l + s + r);

	end = g.end_of(11);
	n = 0;
	for (auto eit = g.begin_of(11); eit != end; ++eit) {
		EXPECT_LE(l, eit.get_target());
		EXPECT_LT(eit.get_target(), l + s + r);
		if (n == 6)
			++n;
		EXPECT_EQ(eit.get_target(), n + l);
		++n;
	}
	EXPECT_EQ(n, s + r);
}

TEST(Graphgen, RandomisedCompleteLSRGraph) {
	size_t l = 5;
	size_t s = 2;
	size_t r = 10;
	graph g = make_randomised_complete_LSR_graph(l, s, r);

	EXPECT_EQ(g.size_vertices(), l + s + r);
	EXPECT_EQ(g.size_edges(), l * (l + s - 1) + s * (l + s + r - 1) + r * (s + r - 1));

	auto end = g.end_of(2);
	size_t n = 0;
	bool ordered = true;
	for (auto eit = g.begin_of(2); eit != end; ++eit) {
		EXPECT_LT(eit.get_target(), l + s);
		if (n == 2)
			++n;
		if (eit.get_target() != n)
			ordered = false;
		++n;
	}
	EXPECT_EQ(n, l + s);
	EXPECT_FALSE(ordered);

	end = g.end_of(6);
	n = 0;
	ordered = true;
	for (auto eit = g.begin_of(6); eit != end; ++eit) {
		if (n == 6)
			++n;
		if (eit.get_target() != n)
			ordered = false;
		++n;
	}
	EXPECT_EQ(n, l + s + r);
	EXPECT_FALSE(ordered);

	end = g.end_of(11);
	n = 0;
	ordered = true;
	for (auto eit = g.begin_of(11); eit != end; ++eit) {
		EXPECT_LE(l, eit.get_target());
		EXPECT_LT(eit.get_target(), l + s + r);
		if (n == 6)
			++n;
		if (eit.get_target() != n + l)
			ordered = false;
		++n;
	}
	EXPECT_EQ(n, s + r);
	EXPECT_FALSE(ordered);
}

/*
TEST(Graphgen, ExactDegreeLSRGraph) {
	size_t l = 5;
	size_t s = 3;
	size_t r = 10;
	size_t d = 6;
	graph g = make_exact_degree_LSR_multigraph(l, s, r, d);

	for (vertex v = 0; v < l; ++v) {
		auto eit = g.begin_of(v);
		auto end = g.end_of(v);
		size_t n = 0;
		for (; eit != end; ++eit) {
			ASSERT_LE(0, eit.get_target());
			ASSERT_LT(eit.get_target(), l + s);
			n++;
		}
		ASSERT_EQ(n, d);
	}
	for (vertex v = l; v < l + s; ++v) {
		auto eit = g.begin_of(v);
		auto end = g.end_of(v);
		size_t n = 0;
		for (; eit != end; ++eit) {
			ASSERT_LE(0, eit.get_target());
			ASSERT_LT(eit.get_target(), l + s + r);
			n++;
		}
		ASSERT_EQ(n, d);
	}
	for (vertex v = l + s; v < l + s + r; ++v) {
		auto eit = g.begin_of(v);
		auto end = g.end_of(v);
		size_t n = 0;
		for (; eit != end; ++eit) {
			ASSERT_LE(l, eit.get_target());
			ASSERT_LT(eit.get_target(), l + s + r);
			n++;
		}
		ASSERT_EQ(n, d);
	}
}
*/

TEST(Graphgen, MakeSplitGraphFromTemplatedGraph) {
	vector<vector<vertex>> regular_input = {
		{1,2},
		{0},
		{0, 1}
	};
	vector<vector<vertex>> split_input = {
		{1}, {2,4},
		{3}, {0},
		{5}, {0, 2}
	};
	graph g(regular_input);
	graph expected_split_g(split_input);
	graph actual_split_g = make_split_graph(g);

	ASSERT_EQ(expected_split_g.size_vertices(), expected_split_g.dfs_parents.size());
	ASSERT_EQ(actual_split_g.size_vertices(), actual_split_g.dfs_parents.size());

	ASSERT_EQ(actual_split_g.size_vertices(), expected_split_g.size_vertices());

	size_t n = actual_split_g.size_vertices();
	for (vertex s = 0; s < n; ++s) {
		auto eit1 = actual_split_g.begin_of(s);
		auto end1 = actual_split_g.end_of(s);
		auto eit2 = expected_split_g.begin_of(s);
		for (; eit1 != end1; ++eit1, ++eit2) {
			EXPECT_EQ(eit1.get_target(), eit2.get_target());
		}
	}

}

TEST(Graphgen, MakeReverseGraphFromTemplatedGraph) {
	vector<vector<vertex>> regular_input = {
		{1, 2},
		{0},
		{0, 1}
	};

	vector<vector<vertex>> reverse_input = {
		{1, 2},
		{0, 2},
		{0}
	};

	graph g(regular_input);
	graph expected_reverse_g(reverse_input);
	graph actual_reverse_g = make_reverse_graph(g);

	ASSERT_EQ(expected_reverse_g.size_vertices(), expected_reverse_g.dfs_parents.size());
	ASSERT_EQ(actual_reverse_g.size_vertices(), actual_reverse_g.dfs_parents.size());

	ASSERT_EQ(actual_reverse_g.size_vertices(), expected_reverse_g.size_vertices());
	ASSERT_EQ(actual_reverse_g.size_edges(), expected_reverse_g.size_edges());

	size_t n = actual_reverse_g.size_vertices();
	for (vertex s = 0; s < n; ++s) {
		auto eit1 = actual_reverse_g.begin_of(s);
		auto end1 = actual_reverse_g.end_of(s);
		auto eit2 = expected_reverse_g.begin_of(s);
		while (true) {
			EXPECT_EQ(eit1.get_target(), eit2.get_target()) << s;
			if (eit1 == end1)
				break;
			++eit1;
			++eit2;
		}
	}
}

TEST(Graphgen, SparsifyGraph) {
	size_t l_size = 30;
	size_t s_size = 3;
	size_t r_size = 40;
	size_t k = s_size + 1;

	graph g = make_complete_LSR_graph(l_size, s_size, r_size);
	graph g_sparse = make_sparse_graph(g, k);

	// EXPECT_TRUE(false) << g_sparse << std::endl; // To print graph.

	size_t n = g_sparse.size_vertices();
	size_t m_sparse = g_sparse.size_edges();
	EXPECT_LE(m_sparse, 2 * (n - 1) * k);

	graph g_split = make_split_graph(g_sparse); // vertex connectivity

	for (vertex s = 0; s < 30; s += 5) {
		vertex t = s + 41;
		vertex s_out = 2 * s + 1;
		vertex t_in = 2 * t;
		EXPECT_EQ(maxflow::ford_fulkerson(g_split, s_out, t_in, k + 1), s_size) << "s: " << s << " t: " << t;
		g_split.reset();
	}
	for (vertex s = 0; s < 30; s += 5) {
		vertex t = (s + 21) % 30;
		vertex s_out = 2 * s + 1;
		vertex t_in = 2 * t;
		EXPECT_GE(maxflow::ford_fulkerson(g_split, s_out, t_in, k + 1), k) << "s: " << s << " t: " << t;
		g_split.reset();
	}
}

TEST(Graphgen, SparseLSRGraph) {
	size_t l_size = 30;
	size_t s_size = 3;
	size_t r_size = 40;
	size_t k = s_size + 1;

	graph g_sparse = make_sparsified_complete_LSR_graph(l_size, s_size, r_size);

	// EXPECT_TRUE(false) << g_sparse << std::endl; // To print graph.

	size_t n = g_sparse.size_vertices();
	size_t m_sparse = g_sparse.size_edges();
	EXPECT_LE(m_sparse, 2 * (n - 1) * k);

	graph g_split = make_split_graph(g_sparse); // vertex connectivity

	// vertices in L are (k-1)-vertex-connected to vertices in R.
	for (vertex s = 0; s < 30; s += 5) {
		vertex t = s + 41;
		vertex s_out = 2 * s + 1;
		vertex t_in = 2 * t;
		EXPECT_EQ(maxflow::ford_fulkerson(g_split, s_out, t_in, k + 1), s_size) << "s: " << s << " t: " << t;
		g_split.reset();
	}
	// vertices in L are k-vertex-connected to vertices in L.
	for (vertex s = 0; s < 30; s += 5) {
		vertex t = (s + 21) % 30;
		vertex s_out = 2 * s + 1;
		vertex t_in = 2 * t;
		EXPECT_GE(maxflow::ford_fulkerson(g_split, s_out, t_in, k + 1), k) << "s: " << s << " t: " << t;
		g_split.reset();
	}
}
