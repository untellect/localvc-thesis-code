#include "pch.h"
#include "localvc.h"
#include "Util.h"

#include <iostream>

namespace localvc {
    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 35
     * Exact version
     *
     * k + 1/mu = 1/epsilon
     *
     * Precondition: g must be at an unchanged state.
     */
    bool local_edge_connectivity_v1(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        size_t stop = 1 * mu * k + 1; // == 1 * mu / epsilon > 1 * mu * k
        std::vector<graph::edge_iterator> visited;

        for (size_t i = 0; i < k; ++i) {
            visited.clear();

            for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                visited.push_back(eit);
                if (visited.size() == stop)
                    break;
            }
            if (visited.size() < stop) {
                std::unordered_set<vertex> visited_vertices;
                visited_vertices.insert(x);
                for (const graph::edge_iterator& e : visited) {
                    visited_vertices.insert(e.get_target());
                }
                // At this point the graph is k-connected if and only if we visited the whole graph.
                if (visited_vertices.size() < g.size_vertices() - k) {
                    cut = visited_vertices;
                    return true;
                }
                return false;
            }

            graph::edge_iterator random_eit = select_randomly(visited);
            g.backtrack_dfs(random_eit); // Start reversing the path at the source, not the end of the edge.0re
            g.reverse_path(random_eit);
        }
        return false;
    }

    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 35
     * Exact version
     *
     * Variation where we generate a point to stop for the first (k-1) iterations rather than collecting visited and sampling a point to reverse from.
     * On the last iteration we run the full max_stop iterations to see if we have a cut.
     * We may get a cut with a larger set S (but smaller L) than the original version.
     *
     * Precondition: g must be at an unchanged state.
     */
    bool local_edge_connectivity_v2(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = true;

        size_t max_stop = 1 * mu * k + 1; // == 1 * mu / epsilon > 1 * mu * k

        for (size_t i = 0; i < k - 1; ++i) {
            size_t stop = random_int(1, max_stop);
            auto eit = g.new_dfs(x);
            for (; eit.is_edge() && stop; --stop) {
                g.advance_dfs(eit);
            }
            if (stop > 0) {
                break; // Go to the k:th iteration. We have a cut.
            }
            g.backtrack_dfs(eit); // Start reversing the path at the source, not the end of the edge.
            g.reverse_path(eit);
        }

        // Last iteration: Collect visited edges.
        auto eit = g.new_dfs(x);
        for (size_t i = 0; eit.is_edge(); g.advance_dfs(eit)) {
            if (++i == max_stop) { // Cut not found
                return false;
            }
        }

        // Cut found.
        auto reachable = get_reachable(g, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k outvertices unaccessible through chance.
        if (reachable.size() < g.size_vertices() - k) {
            cut = reachable;
            return true;
        }
        if constexpr (debug) {
            std::cout << "Found a \"cut\" but can still reach the entire graph." << std::endl;
        }
        return false;
    }


    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 10
     * Exact version
     */
    bool local_edge_connectivity_v3(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        size_t invp = 1 * mu; // At each DFS edge we stop with probability 1/(invp+1)
        std::vector<graph::edge_iterator> visited;
        size_t total_markable_left = 128*mu*k;

        for (size_t i = 0; i < k; ++i) {
            visited.clear();
            graph::edge_iterator yz = g.end_of(x); // Arbitrary invalid edge "NIL", We store edge yz rather than vertex y.
            for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                visited.push_back(eit);
                if (--total_markable_left == 0) {
                    return false; // If we have marked 128 mu k edges, give up.
                }
                if (random_int(1, invp) == 1) {
                    yz = eit; // With probability 1/(8 mu), reverse an edge.
                    break;
                }
            }
            if (yz != g.end_of(x)) { // If yz has been set, then we reverse an edge (otherwise return a cut)
                g.backtrack_dfs(yz); // Start reversing the path at the source, not the end of the edge.
                g.reverse_path(yz);
                continue;
            }

            // If we reach this point the DFS found a stopping point.

            std::unordered_set<vertex> visited_vertices;
            visited_vertices.insert(x);
            for (graph::edge_iterator& e : visited) {
                visited_vertices.insert(e.get_target());
            }
            // At this point the graph is k-connected if and only if we visited the whole graph.
            if (visited_vertices.size() < g.size_vertices() - k) {
                cut = visited_vertices;
                return true;
            }
            return false;
        }
        return false;
    }


    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 10
     * Exact version
     *
     * Variation where the RNG is used once up front rather than many times.
     */
    bool local_edge_connectivity_v4(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        std::vector<vertex> visited;
        //std::unordered_set<vertex> visited_vertices;
        size_t total_markable_left = 128 * mu * k;

        for (size_t i = 0; i < k; ++i) {
            visited.clear();
            visited.push_back(x);
            //visited_vertices.clear();
            //visited_vertices.insert(x);
            graph::edge_iterator yz = g.end_of(x); // Arbitrary invalid edge "NIL", We store edge yz rather than vertex y.
            size_t stop = successes_until_failure(1.0 / (1 * (double)mu)) + 1; // Plus one to stop after a failure rather than before. (if we get a 0 we still visit one edge)
            for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                visited.push_back(eit.get_target());
                //visited_vertices.insert(eit.get_target());
                if (--total_markable_left == 0) {
                    return false; // If we have marked 128 mu k edges, give up.
                }
                if (--stop == 0) {
                    yz = eit; // With probability 1/(8 mu), reverse an edge. (randomness generated up front)
                    break;
                }
            }

            if (yz != g.end_of(x)) {
                g.backtrack_dfs(yz); // Start reversing the path at the source, not the end of the edge.
                g.reverse_path(yz);
                continue;
            }

            // If we reach this point the DFS found a stopping point.

            std::unordered_set<vertex> visited_vertices;
            for (vertex v : visited) {
                visited_vertices.insert(v);
            }

            // At this point the graph is k-connected if and only if we visited the whole graph.
            if (visited_vertices.size() < g.size_vertices() - k) {
                cut = visited_vertices;
                return true;
            }
            return false;
        }
        return false;
    }

    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 10
     * Exact version
     *
     * Variation where visited edges are only collected in the last iteration.
     */
    bool local_edge_connectivity_v5(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        size_t invp = 1 * mu - 1; // At each DFS edge we stop with probability 1/(invp+1)
        size_t total_markable_left = 128 * mu * k;

        for (size_t i = 0; i < k - 1; ++i) {
            graph::edge_iterator yz = g.end_of(x); // Arbitrary invalid edge "NIL", We store edge yz rather than vertex y.
            for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                if (--total_markable_left == 0) {
                    return false; // If we have marked 128 mu k edges, give up.
                }
                if (random_int(0, invp) == 0) {
                    yz = eit; // With probability 1/(8 mu), reverse an edge.
                    break;
                }
            }
            if (yz == g.end_of(x)) {
                break;
            }
            g.backtrack_dfs(yz); // Start reversing the path at the source, not the end of the edge.
            g.reverse_path(yz);
        }

        //std::unordered_set<vertex> visited_vertices;
        //visited_vertices.insert(x);
        for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
            //visited_vertices.insert(eit.get_target());
            if (--total_markable_left == 0) {
                return false; // If we have marked 128 mu k edges, give up.
            }
            if (random_int(0, invp) == 0) {
                return false; // With probability 1/(8 mu), no cut (no need to reverse anymore).
            }
        }

        std::unordered_set<vertex> visited_vertices = get_reachable(g, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        if (visited_vertices.size() < g.size_vertices() - k) {
            cut = visited_vertices;
            return true;
        }
        return false;
    }

    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 10
     * Exact version
     *
     * Variation where visited edges are only collected in the last iteration. (At most mu edges rather than random stop in the last iteration.)
     */
    bool local_edge_connectivity_v6(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        size_t invp = 8 * mu - 1; // At each DFS edge we stop with probability 1/(invp+1)
        size_t total_markable_left = 128 * mu * k;

        for (size_t i = 0; i < k - 1; ++i) {
            graph::edge_iterator yz = g.end_of(x); // Arbitrary invalid edge "NIL", We store edge yz rather than vertex y.
            for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                if (--total_markable_left == 0) {
                    return false; // If we have marked 128 mu k edges, give up.
                }
                if (random_int(0, invp) == 0) {
                    yz = eit; // With probability 1/(8 mu), reverse an edge.
                    break;
                }
            }
            if (yz == g.end_of(x)) {
                break;
            }
            g.backtrack_dfs(yz); // Start reversing the path at the source, not the end of the edge.
            g.reverse_path(yz);
        }

        //std::vector<graph::edge_iterator> visited;
        //std::unordered_set<vertex> visited_vertices;
        //visited_vertices.insert(x);
        if (total_markable_left > mu + 1)
            total_markable_left = mu + 1;
        graph::edge_iterator y = g.end_of(x);
        for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
            //visited.push_back(eit);
            //visited_vertices.insert(eit.get_target());
            if (--total_markable_left == 0) {
                return false; // If we have marked 128 mu k edges, give up.
            }
            //if (visited.size() > mu) {
            //    return{ {}, true }; // We've reached more than mu edges already. Give up. (Maybe add a factor of 2? 4? 8? to catch some lucky cuts?)
            //}
        }

        std::unordered_set<vertex> visited_vertices = get_reachable(g, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        if (visited_vertices.size() < g.size_vertices() - k) {
            cut = visited_vertices;
            return true;
        }
        return false;
    }

    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 35
     * Exact version
     *
     * Version 2 further modified for a smaller max_stop.
     *
     * Precondition: g must be at an unchanged state.
     */
    bool local_edge_connectivity_v7(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        return local_edge_connectivity_v2(g, x, 16*mu, k, cut);
    }

    /*
     * https://arxiv.org/pdf/1910.14344.pdf
     * Page 10
     * Exact version
     *
     * Variation where visited edges are only collected in the last iteration.
     *
     * v5 but corrected
     */
#ifdef ENABLE_MARKING
    bool local_edge_connectivity_v8(graph& g, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        size_t total_markable_left = 16 * mu * k;

        for (size_t i = 0; i < k - 1; ++i) {
            graph::edge_iterator yz = g.end_of(x); // Arbitrary invalid edge "NIL", We store edge yz rather than vertex y.
            // TODO early stopping ?
            size_t stop = successes_until_failure(1.0 / mu) + 1; // Plus one to stop after a failure rather than before. (if we get a 0 we still visit one edge)
            for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
                if (eit.is_marked())
                    continue;
                eit.mark();

                if (--total_markable_left == 0) {
                    g.unmark();
                    return false; // If we have marked 128 mu k edges, give up.
                }
                if (--stop == 0) {
                    yz = eit; // With probability 1/(8 mu), reverse an edge.
                    break;
                }
            }
            if (yz == g.end_of(x)) {
                break;
            }
            g.backtrack_dfs(yz); // Start reversing the path at the source, not the end of the edge.
            g.reverse_path(yz);
        }

        //std::unordered_set<vertex> visited_vertices;
        //visited_vertices.insert(x);
        size_t stop = successes_until_failure(1.0 / mu) + 1; // Plus one to stop after a failure rather than before. (if we get a 0 we still visit one edge)
        for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
            if (eit.is_marked())
                continue;
            eit.mark();

            //if (--total_markable_left == 0) {
            //    g.unmark();
            //    return false; // If we have marked 128 mu k edges, give up.
            //}
            if (--stop == 0) {
                g.unmark();
                return false; // With probability 1/(8 mu), no cut (no need to reverse anymore).
            }
        }
        g.unmark();

        std::unordered_set<vertex> visited_vertices = get_reachable(g, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        if (visited_vertices.size() < g.size_vertices() - k) {
            cut = visited_vertices;
            return true;
        }
        return false;
    }
#endif
}
