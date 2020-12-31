#include "pch.h"

#include <vector>
#include <list>
#include <sstream>
#include "Graph.h"

using namespace std;
using namespace versioned_graph;

// Printing and Constructing graphs (basics, not graphgen).

TEST(MakeGraph, MakeEmptyGraph)
{
    ostringstream ss;
    graph g;

    ss << g;
    EXPECT_EQ(ss.str(), "");
    EXPECT_EQ(g.size_vertices(), 0);
    EXPECT_EQ(g.dfs_parents.size(), 0);
    EXPECT_EQ(g.size_edges(), 0);
}

TEST(MakeGraph, AddVerticesToGraph)
{
    graph g;
    g.add_vertex();
    g.add_vertex();
    g.add_vertex();

    EXPECT_EQ(g.size_vertices(), 3);
    EXPECT_EQ(g.dfs_parents.size(), 3);
    EXPECT_EQ(g.size_edges(), 0);
    {
        string expected =
            "0:\n"
            "1:\n"
            "2:\n";
        ostringstream ss;
        g.dumb_print(ss);
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(MakeGraph, AddVerticesThenEdgeToGraph)
{
    graph g;

    g.add_vertex();
    g.add_vertex();
    g.add_vertex();
    g.add_edge(0, 2);

    EXPECT_EQ(g.size_vertices(), 3);
    EXPECT_EQ(g.dfs_parents.size(), 3);
    EXPECT_EQ(g.size_edges(), 1);
    {
        string expected = "(0,0)(0,2)(0,0)(1,1)(2,2)";
        ostringstream ss;
        g.dump_data(ss);
        EXPECT_EQ(ss.str(), expected);
    }
    {
        string expected = "(0)(2)(3)(4)";
        ostringstream ss;
        g.dump_special(ss);
        EXPECT_EQ(ss.str(), expected);
    }
    {
        string expected =
            "0: 2\n"
            "1:\n"
            "2:\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(MakeGraph, AddEdgeToGraph)
{
    graph g;

    g.add_edge(2, 1);
    EXPECT_EQ(g.size_vertices(), 3);
    EXPECT_EQ(g.dfs_parents.size(), 3);
    EXPECT_EQ(g.size_edges(), 1);
    {
        string expected = "(0,0)(0,0)(1,1)(2,1)(2,2)";
        ostringstream ss;
        g.dump_data(ss);
        EXPECT_EQ(ss.str(), expected);
    }
    {
        string expected = "(0)(1)(2)(4)";
        ostringstream ss;
        g.dump_special(ss);
        EXPECT_EQ(ss.str(), expected);
    }
    {
        string expected =
            "0:\n"
            "1:\n"
            "2: 1\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(MakeGraph, StableAddEdgeToGraph)
{
    graph g;

    g.add_edge_stable(1, 2);
    EXPECT_EQ(g.size_vertices(), 3);
    EXPECT_EQ(g.dfs_parents.size(), 3);
    EXPECT_EQ(g.size_edges(), 1);
    {
        string expected = "(0,0)(0,0)(1,2)(1,1)(2,2)";
        ostringstream ss;
        g.dump_data(ss);
        EXPECT_EQ(ss.str(), expected);
    }
    {
        string expected = "(0)(1)(3)(4)";
        ostringstream ss;
        g.dump_special(ss);
        EXPECT_EQ(ss.str(), expected);
    }
    {
        string expected =
            "0:\n"
            "1: 2\n"
            "2:\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(MakeGraph, StableAddMultipleEdgesToGraph)
{
    graph g;

    g.add_edge_stable(1, 0);
    g.add_edge_stable(1, 2);
    g.add_edge_stable(0, 2);
    EXPECT_EQ(g.size_vertices(), 3);
    EXPECT_EQ(g.dfs_parents.size(), 3);
    EXPECT_EQ(g.size_edges(), 3);
    {
        string expected =
            "0: 2\n"
            "1: 0 2\n"
            "2:\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }
}

TEST(MakeGraph, UnstableAddMultipleEdgesToGraph)
{
    graph g;

    g.add_edge(1, 0);
    g.add_edge(1, 2);
    g.add_edge(0, 2);

    {
        string expected =
            "0: 2\n"
            "1: 2 0\n"
            "2:\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }

    EXPECT_EQ(g.size_vertices(), 3);
    EXPECT_EQ(g.dfs_parents.size(), 3);
    EXPECT_EQ(g.size_edges(), 3);
}

TEST(MakeGraph, TemplatedGraphConstructor) {
    vector<vector<vertex>>  input1 = { {1,2},{0},{0, 1} };
    vector<list<vertex>>    input2 = { {1,2},{0},{0, 1} };
    list<vector<vertex>>    input3 = { {1,2},{0},{0, 1} };

    graph g;
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 0);
    g.add_edge(2, 0);
    g.add_edge(2, 1);
    graph g1(input1);
    graph g2(input2);
    graph g3(input3);

    ostringstream ss, ss1, ss2, ss3;
    ss << g;
    ss1 << g1;
    EXPECT_EQ(ss.str(), ss1.str());
    ss2 << g2;
    EXPECT_EQ(ss.str(), ss2.str());
    ss3 << g3;
    EXPECT_EQ(ss.str(), ss3.str());

    EXPECT_EQ(typeid(g), typeid(g1));
    EXPECT_EQ(typeid(g), typeid(g2));
    EXPECT_EQ(typeid(g), typeid(g3));

    {
        string expected =
            "0: 1 2\n"
            "1: 0\n"
            "2: 0 1\n";
        ostringstream ss;
        ss << g;
        EXPECT_EQ(ss.str(), expected);
    }
}
