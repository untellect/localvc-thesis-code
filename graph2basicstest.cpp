#include "pch.h"

#include <vector>
#include <list>
#include <sstream>
#include "Graph2.h"

using namespace std;
using namespace resettable_graph;

// Printing and Constructing graphs (basics, not graphgen).


TEST(MakeGraphTwo, MakeEmptyGraph)
{
    ostringstream ss;
    graph g;

    ss << g;
    EXPECT_EQ(ss.str(), "");
    EXPECT_EQ(g.size(), 0);
}

TEST(MakeGraphTwo, AddEdgeToGraph)
{
    graph g;

    g.add_edge(0, 2);

    EXPECT_EQ(g.size(), 3);
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

TEST(MakeGraphTwo, AddMultipleEdgesToGraph)
{
    graph g;

    g.add_edge(1, 0);
    g.add_edge(1, 2);
    g.add_edge(0, 2);
    EXPECT_EQ(g.size(), 3);
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

TEST(MakeGraphTwo, TemplatedGraphConstructor) {
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
