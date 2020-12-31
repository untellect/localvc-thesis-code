#include "pch.h"

#include <vector>
#include <list>

#include <sstream>
#include <typeinfo>
#include "Graph2.h"

using namespace std;
using namespace resettable_graph;

void dfs_helper(graph& g, vertex x, ostringstream& ss = ostringstream()) {
    ss << x << ":";

    g.new_search();
    g.visit(x, { x, 0 });
    graph::internal_location dfs = { x, 0 };
    while (true) {
        if (dfs.i >= g[dfs.v].size()) {
            if (dfs.v == x) break;

            dfs = g.internal_parent[dfs.v];
            dfs.i++;
            continue;
        }

        vertex w = g[dfs.v][dfs.i];

        ss << " " << dfs.v << ">" << w;

        if (g.is_visited[w]) {
            dfs.i++;
        }
        else {
            g.visit(w, dfs);
            dfs = { w, 0 };
        }
    }
}

TEST(GraphTwo, dfsIteration) {
    graph g;

    g.add_edge(0, 1); // 0: 1 2 3 4
    g.add_edge(0, 2); // 1:
    g.add_edge(0, 3); // 2: 1 5
    g.add_edge(0, 4); // 3: 2
    g.add_edge(2, 1); // 4: 2
    g.add_edge(2, 5); // 5:
    g.add_edge(3, 2);
    g.add_edge(4, 2);

    {
        ostringstream ss;
        dfs_helper(g, 0, ss);
        string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }
    {
        ostringstream ss;
        dfs_helper(g, 3, ss);
        string expected = "3: 3>2 2>1 2>5";
        EXPECT_EQ(ss.str(), expected);
    }
    {
        ostringstream ss;
        dfs_helper(g, 0, ss);
        string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }
    {
        ostringstream ss;
        dfs_helper(g, 1, ss);
        string expected = "1:";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(GraphTwo, dfsIteration2) {
    graph g;

    g.add_edge(0, 1);
    g.add_edge(0, 5);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 5);

    {
        ostringstream ss;
        dfs_helper(g, 0, ss);
        string expected = "0: 0>1 1>2 2>3 3>4 4>5 0>5";
        EXPECT_EQ(ss.str(), expected);
    }

    {
        ostringstream ss;
        for (vertex v = 4; v != 0; v = g.parent(v)) {
            ss << " " << v;
        }
        string expected = " 4 3 2 1";
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(GraphTwo, dfsAndReversal) {
    graph g;

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;
    g.add_edge(a, b); // a: b c d e
    g.add_edge(a, c); // b:
    g.add_edge(a, d); // c: b f
    g.add_edge(a, e); // d: c
    g.add_edge(c, b); // e: c
    g.add_edge(c, f); // f:
    g.add_edge(d, c);
    g.add_edge(e, c);

    {
        string expected =
            "0: 1 2 3 4\n"
            "1:\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }

    {
        ostringstream ss;
        dfs_helper(g, 0, ss);
        string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reverse_source_to_vertex(1);

    {
        string expected =
            "0: 4 2 3\n"
            "1: 0\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }

    {
        ostringstream ss;
        dfs_helper(g, 0, ss);
        string expected = "0: 0>4 4>2 2>1 1>0 2>5 0>2 0>3 3>2";
        EXPECT_EQ(ss.str(), expected);
    }

    g.reverse_source_to_vertex(5);

    {
        string expected =
            "0: 3 2\n"
            "1: 0\n"
            "2: 1 4\n"
            "3: 2\n"
            "4: 0\n"
            "5: 2\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }

    {
        ostringstream ss;
        dfs_helper(g, 0, ss);
        string expected = "0: 0>3 3>2 2>1 1>0 2>4 4>0 0>2";
        EXPECT_EQ(ss.str(), expected);

        EXPECT_TRUE(g.is_visited[3]);
        EXPECT_EQ(g.parent(3), 0);
        EXPECT_TRUE(g.is_visited[2]);
        EXPECT_EQ(g.parent(2), 3);
        EXPECT_FALSE(g.is_visited[5]);
    }
}

TEST(GraphTwo, pathReversalAndReset) {

    vertex a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;

    graph g;
    g.add_edge(a, b); // 0: 1 2 3 4
    g.add_edge(a, c); // 1:
    g.add_edge(a, d); // 2: 1 5
    g.add_edge(a, e); // 3: 2
    g.add_edge(c, b); // 4: 2
    g.add_edge(c, f); // 5:
    g.add_edge(d, c);
    g.add_edge(e, c);

    dfs_helper(g, 0);
    g.reverse_source_to_vertex(1);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 4 2 3\n"
            "1: 0\n"
            "2: 1 5\n"
            "3: 2\n"
            "4: 2\n"
            "5:\n";
        EXPECT_EQ(ss.str(), expected);
    }

    dfs_helper(g, 0);
    g.reverse_source_to_vertex(1);

    {
        ostringstream ss;
        ss << g;
        string expected =
            "0: 3 2\n"
            "1: 0 2\n"
            "2: 4 5\n"
            "3: 2\n"
            "4: 0\n"
            "5:\n";
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
