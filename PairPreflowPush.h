#pragma once

#include <vector>
#include <unordered_set>
#include <deque>
#include <queue>

// Push-Relabel pair flow.

#define Q_version 1

namespace preflow {
	typedef std::vector<std::vector<size_t>> adjacency_list;

#if Q_version == 2
	class my_priority_queue {
		std::vector<std::vector<size_t>> data;
		std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t> > Q_d;
	public:
		void push(size_t v, std::vector<size_t>& d) {
			const size_t& d_v = d[v];
			while (d_v >= data.size())
				data.push_back({});
			data[d_v].push_back(v);
			if (data[d_v].size() == 1)
				Q_d.push(d_v);
		}
		size_t& top() {
			return data[Q_d.top()].back();
		}
		void pop() {
			data[Q_d.top()].pop_back();
			if (data[Q_d.top()].empty())
				Q_d.pop();
		}
		bool empty() {
			return Q_d.empty();
		}
	};
#endif

	class preflow_helper {
		size_t n;
		size_t s, t;
		adjacency_list SG; // Residual split graph.
#if Q_version == 1
		std::deque<size_t> Q1;
#else if Q_version == 2
		my_priority_queue Q2;
#endif
		std::vector<size_t> current_edge_i;
		std::vector<size_t> excess;
		std::vector<size_t> d;

		// Gap relabelling heuristic.
		std::vector<std::vector<size_t>> active_by_distance;// active_by_distance[i] is a list of vertices at distance i. Does not have to contain vertices confirmed to be disconnected from the sink.

		void push_current(size_t v);
		void relabel(size_t v);

		void run(const adjacency_list& G);

		void bfs_init_d_undirected(const adjacency_list& G, size_t source, size_t target);

		void check_abd(const std::string& id);
	public:
		preflow_helper(const adjacency_list& G, size_t s, size_t t);
		size_t k();
		std::unordered_set<size_t> get_vertex_separator(const adjacency_list& G);
	};

	// Returns a minimum vertex separator S such that G-S has no path s -> t.
	bool pair_vertex_connectivity(const adjacency_list& G, size_t s, size_t t, std::unordered_set<size_t>& min_cut, size_t k = SIZE_MAX);
}