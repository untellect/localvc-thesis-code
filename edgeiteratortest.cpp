#include "pch.h"

#include <vector>
#include <list>

#include <sstream>
#include <typeinfo>
#include "Graph.h"

using namespace std;
using namespace versioned_graph;
typedef graph::node node;
typedef graph::_iterator _iterator;
typedef graph::edge_iterator edge_iterator;

TEST(GraphEdgeIterator, CreateVectorOfNodes)
{
	vector<node> testvec{ {1,2}, {3,4}, {5,6}, {7,8} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;
	_iterator it2 = testvec.begin() + 2;
	_iterator it3 = testvec.begin() + 3;
}

TEST(GraphEdgeIterator, CreateEdgeIterator)
{
	vector<node> testvec{ {1,2}, {3,4}, {5,6}, {7,8} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;
	_iterator it2 = testvec.begin() + 2;
	_iterator it3 = testvec.begin() + 3;

	version_type ver = 1;
	edge_iterator eit(it0, ver);
	EXPECT_EQ(eit.it, it0);
	EXPECT_EQ(eit.ver, ver);
	EXPECT_EQ(eit.get_source(), 1);
	EXPECT_EQ(eit.get_target(), 2);
}

TEST(GraphEdgeIterator, CompareEdgeIterator)
{
	vector<node> testvec{ {1,2}, {3,4}, {5,6}, {7,8} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;
	_iterator it2 = testvec.begin() + 2;
	_iterator it3 = testvec.begin() + 3;

	edge_iterator eit01(it0, 1);
	edge_iterator eit01_2(it0, 1);
	edge_iterator eit11(it1, 1);
	edge_iterator eit02(it2, 2);

	EXPECT_TRUE(eit01 == eit01_2);
	EXPECT_FALSE(eit01 != eit01_2);
	EXPECT_EQ(eit01, eit01_2);
	EXPECT_FALSE(eit01 == eit11);
	EXPECT_TRUE(eit01 != eit11);
	EXPECT_NE(eit01, eit11);
	EXPECT_FALSE(eit01 == eit02);
	EXPECT_TRUE(eit01 != eit02);
	EXPECT_NE(eit01, eit02);
}

TEST(GraphEdgeIterator, IncrementDecrementCreatedEdgeIterator)
{
	vector<node> testvec{ {1,2}, {3,4}, {5,6}, {7,8} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;
	_iterator it2 = testvec.begin() + 2;
	_iterator it3 = testvec.begin() + 3;

	version_type ver = 1;
	edge_iterator eit(it0, ver);
	++eit;
	EXPECT_EQ(eit.it, it1);
	++eit;
	EXPECT_EQ(eit.it, it2);
	++eit;
	EXPECT_EQ(eit.it, it3);
	EXPECT_EQ(eit.ver, ver);
	EXPECT_EQ(eit.get_source(), 7);
	EXPECT_EQ(eit.get_target(), 8);
	--eit;
	EXPECT_EQ(eit.it, it2);
	--eit;
	EXPECT_EQ(eit.it, it1);
	--eit;
	EXPECT_EQ(eit.it, it0);
}

TEST(GraphEdgeIterator, CopyAndIncrement)
{
	vector<node> testvec{ {1,2}, {3,4} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;

	version_type ver = 1;
	edge_iterator eit(it0, ver);
	edge_iterator eit2 = eit;
	EXPECT_EQ(eit, eit2);
	++eit2;
	EXPECT_NE(eit, eit2);
	EXPECT_EQ(eit.it, it0);
	EXPECT_EQ(eit2.it, it1);
}

TEST(GraphEdgeIterator, PlusMinusEdgeIterator)
{
	vector<node> testvec{ {1,2}, {3,4}, {5,6}, {7,8} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;
	_iterator it2 = testvec.begin() + 2;
	_iterator it3 = testvec.begin() + 3;

	version_type ver = 1;
	edge_iterator eit0(it0, ver);
	edge_iterator eit1(it1, ver);
	edge_iterator eit2(it2, ver);
	edge_iterator eit3(it3, ver);

	EXPECT_EQ(eit1, eit0 + 1);
	EXPECT_EQ(eit2, eit0 + 2);
	EXPECT_EQ(eit3, eit1 + 2);

	EXPECT_EQ(eit0, eit2 - 2);
	EXPECT_EQ(eit0, eit3 - 3);

	EXPECT_EQ(eit0 + 1, eit3 - 2);
}

TEST(GraphEdgeIterator, SetNextPrev)
{
	vector<node> testvec{ {1,2}, {3,4}, {5,6}, {7,8}, {9,10} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;
	_iterator it2 = testvec.begin() + 2;
	_iterator it3 = testvec.begin() + 3;
	_iterator it4 = testvec.begin() + 4;

	version_type ver = 1;
	edge_iterator eit(it2, ver);

	eit.setNext(it4);
	EXPECT_EQ((eit + 1).it, it4);
	EXPECT_EQ((eit - 1).it, it1);

	eit.setPrev(it0);
	EXPECT_EQ((eit - 1).it, it0);

	EXPECT_EQ(eit.it, it2);
}

TEST(GraphEdgeIterator, IsEdge)
{
	vector<node> testvec{ {1,2}, {2,2} };
	_iterator it0 = testvec.begin() + 0;
	_iterator it1 = testvec.begin() + 1;

	version_type ver = 1;
	edge_iterator eit(it0, ver);
	ASSERT_NE(eit.get_source(), eit.get_target());
	EXPECT_TRUE(eit.is_edge());
	++eit;
	ASSERT_EQ(eit.get_source(), eit.get_target());
	EXPECT_FALSE(eit.is_edge());
}
