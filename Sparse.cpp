#include "pch.h"

#include "Sparse.h"
#include "Graph.h"
#include <queue>

using namespace versioned_graph;

namespace sparse {
	labelling nagamochi_ibraki_labelling(const std::vector<std::vector<vertex>>& adjacency_list) {
		size_t n = adjacency_list.size();
		std::vector<bool> visited(n, false);
		std::vector<size_t> r(n, 0);
		struct pq_element {
			size_t r;
			size_t x;
			bool operator<(const pq_element& other) const {
				return r < other.r;
			}
		};
		std::priority_queue<pq_element> pq;
		for (vertex x = 0; x < n; x++) pq.push({ 0, x }); // All vertices start unvisited at r=0.

		std::vector<sparse::edge> output = { {n, n, 0} }; // One special node to encode n.

		vertex x;
		while (!pq.empty()) {
			x = pq.top().x;
			vertex r_x = pq.top().r;
			pq.pop();
			if (r[x] != r_x) continue; // ignore duplicates in the priority queue
			if (visited[x]) continue; // Only interested in unvisited x.
			for (vertex y : adjacency_list[x]) {
				if (visited[y]) continue; // Only interested in unvisited y.
				output.push_back({x, y, r[y] + 1 });
				r[y] += 1;
				pq.push({ r[y], y });
			}
			visited[x] = true;
		}
		return { output, x };
	}

	std::vector<std::vector<vertex>> nagamochi_ibraki_adj_list(const labelling& labelling, size_t k) {
		std::vector<std::vector<vertex>> g;
		size_t n = labelling.edges[0].x;
		for (size_t i = 1; i < labelling.edges.size(); i++) {
			if (labelling.edges[i].k <= k) {
				size_t x = labelling.edges[i].x;
				size_t y = labelling.edges[i].y;
				while (x >= g.size()) g.push_back({});
				while (y >= g.size()) g.push_back({});
				g[x].push_back(y);
				g[y].push_back(x);
			}
		}
		return g;
	}

	graph nagamochi_ibraki_graph(const labelling& labelling, size_t k) {
		return graph(nagamochi_ibraki_adj_list(labelling, k));
	}
}
