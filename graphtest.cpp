#include "pch.h"

#include <vector>
#include <list>

#include <sstream>
#include <typeinfo>
#include "Graph.h"

using namespace std;
using namespace versioned_graph;

TEST(Graph, IncrementDecrementAdjacencyIteratorOnSimpleGraph)
{
    graph g;

    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);

    {
        ostringstream ss;

        auto it = g.begin_of(0);
        ss << it.get_source() << ": " << it.get_target();
        ++it;
        ss << " " << it.get_target();
        ++it;
        ss << " " << it.get_target();
        --it;
        ss << " " << it.get_target();
        ++it;
        ss << " " << it.get_target();
        ++it;
        ss << " " << it.get_target();

        auto it2 = g.begin_of(0);
        ss << " " << it2.get_target();

        string expected = "0: 1 2 3 2 3 4 1";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(Graph, ReverseSingleEdge)
{
    graph g;

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(a, d);
    g.add_edge(a, e);
    g.add_edge(c, b);
    g.add_edge(c, f);
    g.add_edge(d, c);
    g.add_edge(e, c);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 1 2 3 4\n"
            "1:\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    graph::edge_iterator eit = g.begin_of(0);

    {
        ostringstream ss;
        ss << eit.get_source() << " -> " << eit.get_target();
        string expected = "0 -> 1";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reverse_edge(eit);

    {
        ostringstream ss;
        ss << eit.get_source() << " -> " << eit.get_target();
        string expected = "1 -> 0";
        EXPECT_EQ(ss.str(), expected);
    }

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 2 3 4\n"
            "1: 0\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reverse_edge(eit);

    {
        ostringstream ss;
        ss << eit.get_source() << " -> " << eit.get_target();
        string expected = "0 -> 1";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(Graph, ReverseEdgesAndReset)
{
    graph g;

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(a, d);
    g.add_edge(a, e);
    g.add_edge(c, b);
    g.add_edge(c, f);
    g.add_edge(d, c);
    g.add_edge(e, c);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 1 2 3 4\n"
            "1:\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    graph::edge_iterator eit = g.begin_of(0);
    g.reverse_edge(eit);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 2 3 4\n"
            "1: 0\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reverse_edge(eit); // After reversing ab the edge iterator refers to the "new" edge ba.

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 2 3 4 1\n"
            "1:\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    --eit;
    g.reverse_edge(eit);
    graph::edge_iterator eit3 = g.begin_of(3);
    g.reverse_edge(eit3);
    graph::edge_iterator eit2 = g.begin_of(2);
    ++eit2;
    g.reverse_edge(eit2);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 2 3 1\n"
            "1:\n"
            "2: 1 3\n"
            "3:\n"
            "4: 2 0\n"
            "5: 2\n";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reverse_edge(eit);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 2 3 1 4\n"
            "1:\n"
            "2: 1 3\n"
            "3:\n"
            "4: 2\n"
            "5: 2\n";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reset();

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 1 2 3 4\n"
            "1:\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(Graph, dfsIteration) {
    graph g;

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(a, d);
    g.add_edge(a, e);
    g.add_edge(c, b);
    g.add_edge(c, f);
    g.add_edge(d, c);
    g.add_edge(e, c);

    {
        ostringstream ss;
        ss << "0:";
        for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }
    {
        ostringstream ss;
        ss << "3:";
        for (auto eit = g.new_dfs(3); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "3: 3>2 2>1 2>5";
        EXPECT_EQ(ss.str(), expected);
    }
    {
        ostringstream ss;
        ss << "0:";
        for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }
    {
        ostringstream ss;
        ss << "1:";
        for (auto eit = g.new_dfs(1); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "1:";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(Graph, dfsIteration2) {
    graph g;

    g.add_edge(0, 1);
    g.add_edge(0, 5);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 5);

    EXPECT_EQ(g.size_vertices(), g.dfs_parents.size());

    {
        ostringstream ss;
        ss << "0:";
        for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "0: 0>1 1>2 2>3 3>4 4>5 0>5";
        EXPECT_EQ(ss.str(), expected);
    }

    {
        ostringstream ss;
        for (auto eit = g.begin_of(4); eit.is_edge(); g.backtrack_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = " 4>5 3>4 2>3 1>2 0>1";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(Graph, dfsAndEdgeReversal) {
    graph g;

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(a, d);
    g.add_edge(a, e);
    g.add_edge(c, b);
    g.add_edge(c, f);
    g.add_edge(d, c);
    g.add_edge(e, c);

    {
        ostringstream ss;
        ss << "0:";
        for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }

    auto eit = g.begin_of(0);
    g.reverse_edge(eit);

    {
        ostringstream ss;
        ss << "0:";
        for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "0: 0>2 2>1 1>0 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }

    eit = g.begin_of(3);
    g.reverse_edge(eit);

    {
        ostringstream ss;
        ss << "0:";
        for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {
            ss << " " << eit.get_source() << ">" << eit.get_target();
        }
        string expected = "0: 0>2 2>1 1>0 2>5 2>3 0>3 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(Graph, pathReversalAndReset) {

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;

    graph g;
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(a, d);
    g.add_edge(a, e);
    g.add_edge(c, b);
    g.add_edge(c, f);
    g.add_edge(d, c);
    g.add_edge(e, c);

    for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {}
    auto eit1 = g.begin_of(2);
    g.reverse_path(eit1);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 1 3 4\n"
            "1: 2\n"
            "2: 5 0\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    for (auto eit = g.new_dfs(0); eit.is_edge(); g.advance_dfs(eit)) {}
    auto eit2 = g.begin_of(2);
    g.reverse_path(eit2);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 3 4\n"
            "1: 0\n"
            "2: 0 1\n"
            "3: 2\n"
            "4: 2\n"
            "5: 2\n";
        EXPECT_EQ(ss.str(), expected);
    }

    g.new_dfs(0);           // Just to invalidate the prevous dfs.
    auto eit3 = g.begin_of(5);
    g.reverse_path(eit3);   // Should not do anything because the version doesn't match.

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 3 4\n"
            "1: 0\n"
            "2: 0 1\n"
            "3: 2\n"
            "4: 2\n"
            "5: 2\n";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reset();

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 1 2 3 4\n"
            "1:\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }
}
