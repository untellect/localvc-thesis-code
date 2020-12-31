#include "pch.h"
#include "localvc.h"
#include "Graphgen.h"

using namespace std;
using namespace localvc;
using namespace versioned_graph;

TEST(LOCALEC_TESTNAME, LineGraphLocalEC)
{
    graph g;
    for (vertex v = 0; v < 10000; ++v) {
        g.add_edge(v, v + 1);
    }
    vertex x = 0;
    size_t mu = 6;
    size_t k = 2;

    unordered_set<vertex> cut;
    bool found_cut;

    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    for (size_t i = 10; i > 0; --i) {
        if (!found_cut) {
            g.reset();
            found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
        }
    }
    bool is_2connected = !found_cut;
    EXPECT_EQ(is_2connected, false); // Not 2-connected.
    EXPECT_EQ(cut.size(), 1); // Cut has size 1.
    EXPECT_NE(cut.find(0), cut.end());  // The cut contains 0.

    k = 1;
    g.reset();
    cut.clear();
    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    bool is_1connected = !found_cut;
    EXPECT_EQ(is_1connected, true); // Is 1-connected.
}

TEST(LOCALEC_TESTNAME, TooSmallGraphLocalEC)
{
    graph g;
    for (vertex v = 0; v < 5; ++v) {
        g.add_edge(v, v + 1);
    }
    vertex x = 0;
    size_t mu = 2; // with these parameters stop will be 32 (>10)
    size_t k = 1;
    
    unordered_set<vertex> cut;
    bool found_cut;

    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    EXPECT_EQ(found_cut, false); // Is 1-connected.
}

TEST(LOCALEC_TESTNAME, DisconnectedGraphLocalEC)
{
    graph g;
    for (vertex v = 0; v < 100; ++v) {
        if (v != 2)
            g.add_edge(v, v + 1);
    }
    vertex x = 0;
    size_t mu = 10;
    size_t k = 1;

    unordered_set<vertex> cut;
    bool found_cut;

    found_cut = LOCALEC_FUNC(g, x, mu, k, cut) || LOCALEC_FUNC(g, x, mu, k, cut) || LOCALEC_FUNC(g, x, mu, k, cut); // boost three times
    EXPECT_EQ(found_cut, true);   // Is not 1-connected.

    vector<vertex> cut_vec(cut.begin(), cut.end());
    ASSERT_EQ(cut.size(), 3);
    sort(cut_vec.begin(), cut_vec.end());
    stringstream ss;
    for (vertex v : cut_vec)
        ss << v << " ";
    EXPECT_EQ(cut_vec[0], 0); // The cut should contain 0 (0_in)
    EXPECT_EQ(cut_vec[1], 1); // The cut should contain 1 (0_out)
    EXPECT_EQ(cut_vec[2], 2); // The cut should contain 2 (1_in)
}

TEST(LOCALEC_TESTNAME, LoneLCliqueLSRLocalEC)
{
    ostringstream ss;

    size_t l_size = 1;
    size_t s_size = 3;
    size_t r_size = 20;
    graph g = make_randomised_complete_LSR_graph(l_size, s_size, r_size);

    vertex x = 0;
    size_t mu = 3;
    size_t k = 4;

    unordered_set<vertex> cut;
    bool found_cut;

    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    for (int i = 0; i < 20 && !found_cut; ++i) { // Boosting probability.
        g.reset();
        found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    }

    EXPECT_EQ(found_cut, true); // Not 4-connected.
    EXPECT_EQ(cut.size(), 1); // Cut has size 1.
    EXPECT_NE(cut.find(0), cut.end()); // The cut should contain 0

    k = 3;
    g.reset();
    cut.clear();

    found_cut = LOCALEC_FUNC(g, x, mu, k, cut);
    EXPECT_EQ(found_cut, false); // Is 3-connected.
}

TEST(LOCALEC_TESTNAME, SplitOfSmallLCliqueLSRLocalEC)
{
    size_t n_L = 2;
    size_t n_S = 5;
    size_t n_R = 300;

    graph g = make_complete_LSR_graph(n_L, n_S, n_R);
    graph split_g = make_split_graph(g);

    vertex x_out = 1;
    size_t mu = n_L * (n_L + n_S) + 1;
    size_t k = n_S + 1;

    unordered_set<vertex> cut;
    bool found_cut;

    found_cut = LOCALEC_FUNC(split_g, x_out, mu, k, cut);
    for (int i = 0; i < 20; ++i) { // Boosting probability.
        if (found_cut)
            break;
        split_g.reset();
        found_cut = LOCALEC_FUNC(split_g, x_out, mu, k, cut);
    }

    bool is_kconnected = !found_cut;
    ASSERT_EQ(is_kconnected, false); // Not k-connected.
    ASSERT_EQ(cut.size(), 9) << *(cut.begin()); // Cut has size 2.

    {
        vector<vertex> cut_vec(cut.begin(), cut.end());
        sort(cut_vec.begin(), cut_vec.end());
        stringstream ss;
        for (vertex v : cut_vec)
            ss << v << " ";
        EXPECT_EQ(cut_vec[0], 0); // The cut should contain 0 (0_in)
        EXPECT_EQ(cut_vec[1], 1); // The cut should contain 1 (0_out)
        EXPECT_EQ(cut_vec[2], 2); // The cut should contain 2 (1_in)
        EXPECT_EQ(cut_vec[3], 3); // The cut should contain 3 (1_out)

        EXPECT_EQ(cut_vec[4], 4); // The cut should contain 4 (2_in)
        EXPECT_EQ(cut_vec[5], 6); // The cut should contain 6 (3_in)
        EXPECT_EQ(cut_vec[6], 8); // The cut should contain 8 (4_in)
        EXPECT_EQ(cut_vec[7], 10); // The cut should contain 10 (5_in)
        EXPECT_EQ(cut_vec[8], 12); // The cut should contain 12 (6_in)
    }

    k = 5;
    split_g.reset();
    cut.clear();

    found_cut = LOCALEC_FUNC(g, x_out, mu, k, cut);
    vector<vertex> cut_vec(cut.begin(), cut.end());
    sort(cut_vec.begin(), cut_vec.end());
    stringstream ss;
    for (vertex v : cut_vec)
        ss << v << " ";
    EXPECT_EQ(found_cut, false) << ss.str(); // Is 5-connected.
}

