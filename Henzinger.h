#pragma once

#include "Sparse.h"
#include "Util.h"

#include <vector>
#include <queue>
#include <numeric>
#include <unordered_set>
#include <chrono>

// 1 for vector, 2 for set, 3 for vector with index lookup for efficient swap-pop erase
#define W_BY_DISTANCE_TYPE 3

namespace henzinger {
	typedef std::vector<std::vector<size_t>> adjacency_list;

	struct henzinger_subresult {
		size_t i;
		size_t k;
		size_t m;
		std::chrono::steady_clock::duration t;
	};
	typedef std::unordered_set<size_t> (henzinger_vc_fn)(const adjacency_list& g, size_t& x, std::vector<henzinger_subresult>& subresults);
	henzinger_vc_fn vc_undirected_linear_random, vc_undirected_linear_unsparsified_random, vc_undirected_linear, vc_undirected_linear_unsparsified, vc_directed_linear, vc_undirected_doubling_random, vc_undirected_doubling2_random, vc2_undirected_doubling2_random;

	/*
	 * Encapsulates the logic for min vc split (attempt 2)
	 */
	class HenzingerVC {
		size_t n;
		adjacency_list SG; // Residual split graph.
		std::vector<size_t> current_edge_i;

		std::vector<bool> in_W;
		std::deque<size_t> W_and_overflowing; // Vertices here are guaranteed to be in W but not different from the sink t.

#if W_BY_DISTANCE_TYPE == 1
		std::deque<std::vector<size_t>> W_by_distance;
#elif W_BY_DISTANCE_TYPE == 2
		std::deque<std::unordered_set<size_t>> W_by_distance;
#elif W_BY_DISTANCE_TYPE == 3
		std::vector<size_t> wbd_i; // W_by_distance[d[v] - d_min_W][wbd_i[v]] = v  
		std::deque<std::vector<size_t>> W_by_distance;
#endif
		size_t d_min_W = 0;

		std::vector<size_t> best_W;
		std::vector<size_t> best_W_neighbors;
		size_t best_W_indegree = SIZE_MAX;

		std::vector<size_t> d;
		std::vector<size_t> excess;

		std::vector<std::vector<size_t>> D = { std::vector<size_t>() };
		size_t gamma = 0;
		std::vector<size_t> d_min_D_and_T = { SIZE_MAX };
		std::vector<size_t> d_min_D = { SIZE_MAX }; // These help with merging D[gamma] into W.
		std::vector<size_t> d_max_D = { 0 };

		size_t t;

		std::vector<size_t>& S() { return D[0]; };
		bool in_T(size_t v) { return (v & 1) == 0; } // v is even/invertex

		void new_sink();
		void push_from(size_t v);
		void relabel(size_t v);
		void create_dormant(size_t v);
		void create_singleton_dormant_or_join(size_t v);

		void check(std::string id);
		void checkWBD_i(std::string id);
		void checkDconsecutiveness(std::string id);
		void checkDormancy(std::string id);
		void checkDValidity(std::string id);
	public:
		/* procedure initialize (plus constructing the split graph) */
		HenzingerVC(const adjacency_list& G, size_t s);

		friend std::unordered_set<size_t> min_vc_split(const adjacency_list& G, size_t s);
		friend std::unordered_set<size_t> min_vc_split_v2(const adjacency_list& G, size_t s);
		friend std::unordered_set<size_t> min_vc_split_pair(const adjacency_list& G, size_t s, size_t t); // bad?
	};
}