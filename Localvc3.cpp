#include "pch.h"
#include "localvc3.h"
#include "Util.h"

#include <deque>

namespace localvc3 {
	// page 10 version, precalculate stopping point.
	bool local_edge_connectivity_v8(graph& sg, vertex x, size_t nu, size_t k, std::unordered_set<vertex>& cut) {
		throw "Not yet implemented.";

		double stop_probability = 1.0 / nu;
		size_t total_markable_left = 128 * nu * k;

		for (size_t i = 0; i < k; i++) {
			size_t edges_left = successes_until_failure(stop_probability) + 1; // Plus one to stop after a failure rather than before. (if we get a 0 we still visit one edge)

			vertex v = x;

			// DFS
			while (true) {
				// New edges
				if (sg.orig[v].size() > sg.orig_i[v]) {
					vertex w = sg.orig[v][sg.orig_i[v]];

					if (!sg.visited[w]) {
						sg.visit(w);
						sg.parent_v[w] = v;
						sg.parent_i[w] = sg.orig_i[v];

						v = w;
					}
					else {
						sg.orig_i[v]++;
					}

					if (--edges_left == 0) {
						break;
					}

					// Go to next vertex.
					// If out of vertices here, backtrack and advance on that list.
					// If was already out of vertices (i >= size), then result is (i > size) and we will leave the "new edges" while loop.
					while (++sg.orig_i[v] == sg.orig[v].size()) {
						v = sg.parent_v[v];
					}
				}

				if (edges_left == 0) {
					break;
				}

				// Old edges
				while (sg.revisit_i < sg.visited_vertices.size()) {

				}
			}

		}
	}


	// page 10 version, precalculate stopping point.
	// BFS based for efficiency
	// Only revisit edges if the search tree can not be extended with new edges.
	bool local_edge_connectivity_v10(graph& sg, vertex x, size_t nu, size_t k, std::unordered_set<vertex>& cut) {
		double stop_probability = 1.0 / nu;
		size_t total_markable_left = 128 * nu * k;

		for (size_t i = 0; i < k; i++) {
			size_t edges_left = successes_until_failure(stop_probability) + 1; // Plus one to stop after a failure rather than before. (if we get a 0 we still visit one edge)

			vertex v = x;

			// BFS
			while (true) {
				// New edges
				while (sg.orig[v].size() > sg.orig_i[v]) {
					vertex w = sg.orig[v][sg.orig_i[v]];

					// V
					if (!sg.visited[w]) {
						sg.visit(w);
						sg.parent_v[w] = v;
						sg.parent_i[w] = sg.orig_i[v];

						v = w;
					}

					if (--edges_left == 0) {
						break;
					}

					// Go to next vertex.
					// If out of vertices here, backtrack and advance on that list.
					// If was already out of vertices (i >= size), then result is (i > size) and we will leave the "new edges" while loop.
					while (++sg.orig_i[v] == sg.orig[v].size()) {
						v = sg.parent_v[v];
					}
				}

				if (edges_left == 0) {
					break;
				}

				// Old edges
				while (sg.revisit_i < sg.visited_vertices.size()) {

				}
			}

		}

		return false;
	}
}