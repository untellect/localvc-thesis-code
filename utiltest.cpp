#include "pch.h"
#include "Util.h"

using namespace std;
using namespace versioned_graph;

// tradeoff: We lose some information on which line the issue happened but test files are more readable.
// Does this exist natively in google test?
void expect_set_equal(unordered_set<vertex> S, unordered_set<vertex> expected, string s) {
	EXPECT_EQ(S.size(), expected.size()) << s;
	for (vertex v : expected) {
		EXPECT_NE(S.find(v), S.end()) << "v = " << v << endl << s;
	}
}

TEST(Util, getReachable)
{
	graph g1, g2;
	for (vertex v = 0; v < 10; ++v) {
		if(v != 5)
			g1.add_edge(v, v + 1);
		g2.add_edge(v, v + 1);
	}
	g2.reverse_edge(g2.begin_of(5));

	unordered_set<vertex> expected{ 0, 1, 2, 3, 4, 5 };
	unordered_set<vertex> r1 = get_reachable(g1, 0); // Should be connected up to 5
	unordered_set<vertex> r2 = get_reachable(g2, 0); // Should be reachable no further than 5 because (5->6) is reversed.

	expect_set_equal(r1, expected, "r1");
	expect_set_equal(r2, expected, "r2");
}

TEST(Util, getNeighbors)
{
	vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;

	graph g;
	g.add_edge(a, b); g.add_edge(a, c); g.add_edge(a, d); g.add_edge(a, e);
	g.add_edge(c, b); g.add_edge(c, f);
	g.add_edge(d, c);
	g.add_edge(e, c);

	expect_set_equal(get_neighbors(g, { }), { }, "empty set");
	expect_set_equal(get_neighbors(g, { b }), { }, "set with no neighbors");
	expect_set_equal(get_neighbors(g, { a }), {b, c, d, e}, "{a}");
	expect_set_equal(get_neighbors(g, { d, e }), { c }, "{d, e}");
	expect_set_equal(get_neighbors(g, { c, e }), { b, f }, "{c, e}");
	expect_set_equal(get_neighbors(g, { a, c }), { b, d, e, f}, "{a, c}");
}
