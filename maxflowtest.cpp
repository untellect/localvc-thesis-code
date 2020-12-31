#include "pch.h"
#include "MaxFlow.h"
#include "Graphgen.h"
#include "Util.h"

using namespace std;
using namespace maxflow;
using namespace versioned_graph;

TEST(Maxflow, SingleEdge)
{
    graph g;
    g.add_edge(0, 1);

    EXPECT_EQ(ford_fulkerson(g, 0, 1, 1), 1);
    auto S = get_reachable(g, 0);
    EXPECT_EQ(S.size(), 1);
    g.reset();
    auto N = get_neighbors(g, S);
    EXPECT_EQ(N.size(), 1);


    EXPECT_EQ(ford_fulkerson(g, 0, 1, 1), 1);
    g.reset();
    EXPECT_EQ(ford_fulkerson(g, 1, 0, 1), 0);
}


TEST(Maxflow, LoneLCliqueLSRLocalEC)
{
    size_t l_size = 1;
    size_t s_size = 3;
    size_t r_size = 20;
    graph g = make_randomised_complete_LSR_graph(l_size, s_size, r_size);

    vertex x = 0;
    vertex y = 10;
    vertex z = 20;
    size_t k = s_size;
    size_t ans = ford_fulkerson(g, x, z, 100);
    EXPECT_EQ(k, ans);
    g.reset();
    ans = ford_fulkerson(g, y, z, 100); // Highly connected pair of vertices.
    EXPECT_LT(k, ans);
    g.reset();
    ans = ford_fulkerson(g, z, x, 100);
    EXPECT_EQ(k, ans);
}

TEST(Maxflow, NonLoneLSplitGraphLSRLocalEC)
{
    size_t l_size = 20;
    size_t s_size = 3;
    size_t r_size = 20;
    graph g = make_randomised_complete_LSR_graph(l_size, s_size, r_size);
    graph g_split = make_split_graph(g);

    vertex x_out = 2 * 0 + 1;
    vertex y_in = 2 * 10;
    vertex z_in = 2 * 30;
    size_t k = s_size;
    size_t ans = ford_fulkerson(g_split, x_out, z_in, 100);
    EXPECT_EQ(k, ans);
    g.reset();
    ans = ford_fulkerson(g_split, x_out, y_in, 100); // Highly connected pair of vertices.
    EXPECT_LT(k, ans);
}