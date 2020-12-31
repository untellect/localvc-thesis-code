#include "pch.h"
#include "localvc2.h"
#include "Graphgen.h"

using namespace std;
using namespace localvc2;
using namespace resettable_graph;

TEST(LOCALEC_TESTNAME, LineGraphLocalEC)
{
    graph g;
    for (vertex v = 0; v < 300; ++v) {
        g.add_edge(v, v + 1);
    }
    vertex x = 0;
    size_t mu = 8;
    size_t k = 2;

    unordered_set<vertex> cut;
    bool found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    for (size_t i = 20; i > 0; --i) {
        if (!found_cut) {
            g.reset();
            found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
        }
    }
    EXPECT_EQ(found_cut, true); // Not 2-connected.
    EXPECT_EQ(cut.size(), 1); // Cut has size 1.
    if (!cut.empty()) {
        EXPECT_TRUE(cut.find(0) != cut.end()); // Cut contains only 0.
    }

    k = 1;
    g.reset();
    cut.clear();

    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    EXPECT_EQ(found_cut, false); // Is 1-connected.
    EXPECT_EQ(cut.size(), 0); // Cut has size 0.
}

TEST(LOCALEC_TESTNAME, TooSmallGraphLocalEC)
{
    graph g;
    for (vertex v = 0; v < 6; ++v) {
        g.add_edge(v, v + 1);
    }
    vertex x = 0;
    size_t mu = 20; // with these parameters stop will be way too large
    size_t k = 1;

    unordered_set<vertex> cut;
    bool found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    EXPECT_EQ(found_cut, false); // Is 1-connected.
    if (found_cut) {
        std::cout << "cut:";
        for (vertex v : cut)
            std::cout << " " << v;
        std::cout << std::endl;
    }

    // Do not test k > 1. mu is too large.
}

TEST(LOCALEC_TESTNAME, DisconnectedGraphLocalEC)
{
    graph g;
    for (vertex v = 0; v < 200; ++v) {
        if (v != 2)
            g.add_edge(v, v + 1);
    }
    vertex x = 0;
    size_t mu = 10;
    size_t k = 1;

    unordered_set<vertex> cut;
    bool found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    EXPECT_EQ(found_cut, true); // Is not 1-connected.
    EXPECT_EQ(cut.size(), 3);
}

TEST(LOCALEC_TESTNAME, LoneLCliqueLSRLocalEC)
{
    ostringstream ss;

    size_t l_size = 1;
    size_t s_size = 3;
    size_t r_size = 20;
    auto& adj = versioned_graph::make_randomised_complete_LSR_graph_adj_list(l_size, s_size, r_size);
    graph g(adj);

    vertex x = 0;
    size_t mu = 3;
    size_t k = 4;
    unordered_set<vertex> cut;
    bool found_cut;
    for (int i = 0; i < 20; ++i) { // Boosting probability.
        found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
        if (found_cut)
            break;
    }

    EXPECT_EQ(found_cut, true); // Not 4-connected.
    EXPECT_EQ(cut.size(), 1); // Cut (L) has size 2.
    if (!cut.empty()) {
        // The cut should contain 0 and 1.
        EXPECT_NE(cut.find(0), cut.end());
    }

    k = 3;
    g.reset();
    cut.clear();
    
    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    EXPECT_EQ(found_cut, false); // Is 3-connected.
}
