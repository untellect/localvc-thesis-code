#include "pch.h"
#include "Graph.h"

// No prerequisite tests.

using namespace versioned_graph;
typedef graph::node node;

TEST(GraphNode, Create)
{
	node n(1, 2);
	// Default values.
	EXPECT_EQ(n.s, 1);
	EXPECT_EQ(n.t, 2);
	EXPECT_EQ(n.next_v, 0);
	EXPECT_EQ(n.prev_v, 0);
	EXPECT_EQ(n.reversed_v, 0);
}

TEST(GraphNode, SourceAndTarget)
{
	node n(1, 2);
	version_type ver = 1;
	EXPECT_EQ(n.get_source(ver), 1);
	EXPECT_EQ(n.get_target(ver), 2);
}

TEST(GraphNode, ReverseDirection)
{
	node n(1, 2);
	version_type ver = 1;
	// Back and forth a few times.
	for (int i = 0; i < 3; ++i) {
		n.reverse_direction(ver);
		EXPECT_EQ(n.get_source(ver), 2);
		EXPECT_EQ(n.get_target(ver), 1);
		n.reverse_direction(ver);
		EXPECT_EQ(n.get_source(ver), 1);
		EXPECT_EQ(n.get_target(ver), 2);
	}
}

TEST(GraphNode, ReverseAndReset)
{
	node n(1, 2);
	version_type ver = 1;
	n.reverse_direction(ver);
	++ver; // reset
	EXPECT_EQ(n.get_source(ver), 1);
	EXPECT_EQ(n.get_target(ver), 2);
	n.reverse_direction(ver);
	EXPECT_EQ(n.get_source(ver), 2);
	EXPECT_EQ(n.get_target(ver), 1);
	n.reverse_direction(ver);
	EXPECT_EQ(n.get_source(ver), 1);
	EXPECT_EQ(n.get_target(ver), 2);
	n.reverse_direction(ver);
	EXPECT_EQ(n.get_source(ver), 2);
	EXPECT_EQ(n.get_target(ver), 1);
	++ver; // reset
	EXPECT_EQ(n.get_source(ver), 1);
	EXPECT_EQ(n.get_target(ver), 2);
	++ver; // reset
	EXPECT_EQ(n.get_source(ver), 1);
	EXPECT_EQ(n.get_target(ver), 2);
}