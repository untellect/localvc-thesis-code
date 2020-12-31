#include "pch.h"
#include "Localvc.h"
#include "Util.h"

#include <iostream>

namespace localvc {
	enum localec_main_version {
		page10,
		page35
	};
	template<
        localec_main_version main_version = page35,
        bool postpone_collecting_visited = true,
        bool precalculate_randomness = true,
        // WARNING: For page35 version 'postpone_collecting_visited' needs to imply 'precalculate_randomness'
        typename = std::enable_if_t<main_version != page35 || !postpone_collecting_visited || precalculate_randomness, void>
    >
	bool local_edge_connectivity_templated(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {

        std::vector<graph::edge_iterator> visited_edges;

        size_t invp = 8 * mu;                       // for page 10 version
        size_t total_markable_left = 128 * mu * k;  // for page 10 version
        size_t dfs_max_size = 1 * mu * k;           // for page 35 version

        for (size_t i = 0; i < k; ++i) {
            if constexpr(!postpone_collecting_visited)
                visited_edges.clear();

            size_t dfs_left;
            if constexpr (main_version == page10) {
                if (precalculate_randomness) {
                    dfs_left = successes_until_failure(1.0 / (8 * (double)mu)) + 1;
                }
            }
            else if constexpr (main_version == page35) {
                if (precalculate_randomness) {
                    dfs_left = random_int(1, dfs_max_size);
                }
            }

            for (graph::edge_iterator eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                if constexpr (!postpone_collecting_visited) {
                    visited_edges.push_back(eit);
                }
                if constexpr (main_version == page35 || precalculate_randomness) { // In the 'precalculate_randomness' case page10 and page35 work the same way at this point.
                    if (--dfs_left == 0) {
                        if (i < k - 1) { // If we have marked edges in this DFS up until the stopping point, reverse from a random edge...
                            if constexpr (!precalculate_randomness) {
                                eit = select_randomly(visited_edges); // Either this or the line 'dfs_left = random_int(1, dfs_max_size);' earlier.
                            }
                            g.backtrack_dfs(eit);
                            g.reverse_path(eit);
                            break; // Next DFS
                        }
                        else {
                            return false; // ...except for the last iteration, where this means we did not find a cut.
                        }
                    }
                }
                else if constexpr (main_version == page10) {
                    if (--total_markable_left == 0) { // If we have marked 128 mu k edges in total, give up.
                        return false; 
                    }
                    if (random_int(1, invp) == 1) { // With probability 1/(8 mu), reverse from current edge...
                        if (i < k - 1) {
                            g.backtrack_dfs(eit);
                            g.reverse_path(eit);
                            break; // Next DFS
                        }
                        else {
                            return false; // ...except for the last iteration, where this means we did not find a cut.
                        }
                        
                    }
                }
            }
        }
        // If we exit the loop here, then the last DFS ended because of "eit.is_edge()", which means we ran out of edges. There should be a cut.

        std::unordered_set<vertex> visited_vertices;
        if constexpr (postpone_collecting_visited) { // Redo the last DFS to collect the visited vertices.
            visited_vertices = get_reachable(g, x); 
        }
        else if constexpr (!postpone_collecting_visited) { // Collect visited vertices from the visited edges.
            visited_vertices.insert(x); 
            for (const graph::edge_iterator& e : visited_edges) {
                visited_vertices.insert(e.get_target());
            }
        }

        // At this point the graph is k-connected if and only if we visited the whole graph.
        if (visited_vertices.size() < g.size_vertices() - k) {
            cut = visited_vertices;
            return true;
        }
        return false;
	}
}
