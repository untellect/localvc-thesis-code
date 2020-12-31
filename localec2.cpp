#include "pch.h"
#include "localvc2.h"
#include "Util.h"

namespace localvc2 {

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
    bool local_edge_connectivity_v2(graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = false;

        size_t max_stop = 1 * mu * k + 1; // == 1 * mu / epsilon > 1 * mu * k

        for (size_t iter = 0; iter < k; ++iter) {
            size_t edges_left = random_int(1, max_stop) + 1;
            if (iter == k - 1) edges_left = max_stop + 1;

            sg.new_search();
            sg.visit(x, { x, 0 });
            graph::internal_location dfs = { x, 0 };
            while (true) {
                if (dfs.i >= sg[dfs.v].size()) {
                    if (dfs.v == x) {
                        if constexpr (debug) {
                            std::cout << "\t\t\tRan out of edges" << std::endl;
                        }
                        break;
                    }

                    dfs = sg.internal_parent[dfs.v];
                    dfs.i++;
                    continue;
                }

                if (--edges_left == 0) break; // If we reach this point we are visiting an edge.

                vertex w = sg[dfs.v][dfs.i];
                if (sg.is_visited[w]) {
                    dfs.i++;
                }
                else {
                    sg.visit(w, dfs);
                    dfs = { w, 0 };
                }
            }

            if (edges_left > 0) {
                if constexpr (debug) {
                    std::cout << "\t\t\tstopped with edges left" << std::endl;
                }
                break; // Go to the k:th iteration. We have a cut.
            }

            // On the last iteration, return no cut rather than reverse.
            if (iter == k - 1) {
                if constexpr (debug) {
                    std::cout << "\t\t\tlast iteration had enough edges" << std::endl;
                }
                return false;
            }
                
            sg.reverse_source_to_vertex(dfs.v);
        }

        // If we reach this point, then we left the above loop with (edges_left > 0). There is a cut.
        auto reachable = std::unordered_set<vertex>(sg.visited_vertices.begin(), sg.visited_vertices.end());
        //auto reachable = get_reachable(sg, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k-1 outvertices unaccessible through chance.
        if constexpr (debug) {
            std::cout << "\t\t\treachable.size: " << reachable.size() << std::endl;
            std::cout << "\t\t\tMaxsize: " << sg.size() - k << std::endl;
        }
        if (reachable.size() < sg.size() - k) {
            cut = reachable;
            if constexpr (debug) {
                std::cout << "\t\t\tL: " << cut.size() << std::endl;
            }
            return true;
        }
        return false;
    }
    bool local_edge_connectivity_v1(graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = false;

        size_t max_stop = 1 * mu * k + 1; // == 1 * mu / epsilon > 1 * mu * k

        for (size_t iter = 0; iter < k; ++iter) {
            std::vector<vertex> sample_pool;
            size_t edges_left = max_stop;

            sg.new_search();
            sg.visit(x, { x, 0 });
            graph::internal_location dfs = { x, 0 };
            while (true) {
                if (dfs.i >= sg[dfs.v].size()) {
                    if (dfs.v == x) {
                        if constexpr (debug) {
                            std::cout << "\t\t\tRan out of edges" << std::endl;
                        }
                        break;
                    }

                    dfs = sg.internal_parent[dfs.v];
                    dfs.i++;
                    continue;
                }

                sample_pool.push_back(dfs.v);
                if (--edges_left == 0) break;

                vertex w = sg[dfs.v][dfs.i];
                if (sg.is_visited[w]) {
                    dfs.i++;
                }
                else {
                    sg.visit(w, dfs);
                    dfs = { w, 0 };
                }
            }

            if (sample_pool.size() < max_stop) {
                if constexpr (debug) {
                    std::cout << "\t\t\tstopped with edges left" << std::endl;
                }
                break; // Go to the k:th iteration. We have a cut.
            }

            // On the last iteration, return no cut rather than reverse.
            if (iter == k - 1) {
                if constexpr (debug) {
                    std::cout << "\t\t\tlast iteration had enough edges" << std::endl;
                }
                return false;
            }

            vertex v = select_randomly(sample_pool);
            sg.reverse_source_to_vertex(v);
        }

        // If we reach this point, then we left the above loop with (edges_left > 0). There is a cut.
        auto reachable = std::unordered_set<vertex>(sg.visited_vertices.begin(), sg.visited_vertices.end());
        //auto reachable = get_reachable(sg, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k-1 outvertices unaccessible through chance.
        if constexpr (debug) {
            std::cout << "\t\t\treachable.size: " << reachable.size() << std::endl;
            std::cout << "\t\t\tMaxsize: " << sg.size() - k << std::endl;
        }
        if (reachable.size() < sg.size() - k) {
            cut = reachable;
            if constexpr (debug) {
                std::cout << "\t\t\tL: " << cut.size() << std::endl;
            }
            return true;
        }
        return false;
    }

    /*
     * Entirely different version. Counting vertex degrees rather than directly counting edges.
     */
    bool local_edge_connectivity_degreecount(graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = false;
        const bool debug2 = false;

        if constexpr (debug || debug2) {
            std::cout << "\t\t\tStarting localec degreecount" << std::endl;
        }

        size_t max_stop = 1 * mu * k + 1; // == 1 * mu / epsilon > 1 * mu * k

        if (sg[x].size() >= max_stop)
            return false;

        for (size_t iter = 0; iter < k; ++iter) {
            size_t edges_left = random_int(sg[x].size() + 1, max_stop);
            if (iter == k - 1) edges_left = max_stop;

            sg.new_search();
            sg.visit(x, { x, 0 });
            graph::internal_location dfs = { x, 0 };
            while (edges_left > 0) {
                while (dfs.i < sg[dfs.v].size() && sg.is_visited[sg[dfs.v][dfs.i]]) {
                    dfs.i++;
                }
                if (dfs.i >= sg[dfs.v].size()) {
                    if (dfs.v == x) {
                        if constexpr (debug) {
                            std::cout << "\t\t\tRan out of edges" << std::endl;
                        }
                        break;
                    }

                    dfs = sg.internal_parent[dfs.v];
                    dfs.i++;
                    continue;
                }

                vertex w = sg[dfs.v][dfs.i];

                sg.visit(w, dfs);
                dfs = { w, 0 };

                if constexpr (debug2) {
                    std::cout << "\t\t\tvisiting " << w << std::endl;
                }

                // degree counting here instead of edge counting.
                if (sg[w].size() >= edges_left) {
                    if constexpr (debug2) {
                        std::cout << "\t\t\tenough edges at " << dfs.v << std::endl;
                    }
                    edges_left = 0; // Search went long enough.
                }
                else {
                    edges_left -= sg[w].size(); // Not done.
                }

                if constexpr (debug2) {
                    std::cout << "\t\t\tedges left: " << edges_left << std::endl;
                }
            }

            if (edges_left > 0) {
                if constexpr (debug) {
                    std::cout << "\t\t\tstopped with edges left" << std::endl;
                }
                break; // We have a cut.
            }

            // On the last iteration, return no cut rather than reverse.
            if (iter == k - 1) {
                if constexpr (debug) {
                    std::cout << "\t\t\tlast iteration had enough edges" << std::endl;
                }
                return false;
            }

            sg.reverse_source_to_vertex(dfs.v);
        }

        // If the last iteration did not return false we have a cut that corresponds to the visited vertices.
        auto reachable = std::unordered_set<vertex>(sg.visited_vertices.begin(), sg.visited_vertices.end());

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k-1 outvertices unaccessible through chance.
        if constexpr (debug) {
            std::cout << "\t\t\treachable.size: " << reachable.size() << std::endl;
            std::cout << "\t\t\tMaxsize: " << sg.size() - k << std::endl;
        }
        if (reachable.size() < sg.size() - k) {
            cut = reachable;
            if constexpr (debug) {
                std::cout << "\t\t\tL: " << cut.size() << std::endl;
            }
            return true;
        }
        return false;
    }
    

















    bool local_edge_connectivity_marking_degreecount(graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = false;
        const bool debug2 = false;
        const bool debug3 = false;

        const bool debug4 = false; // Reason for failure
         
        {
            size_t d_x = sg[x].size();
            if (mu < d_x) {
                if constexpr (debug4) {
                    std::cout << "\t\t\ttoo low volume" << std::endl;
                }
                return false;
            }
            mu -= d_x; // The outgoing edges of x are *obviously* part of the cut, so let's only count other edges.
        }

        size_t total_markable_left = 2 * mu * k; // 2 is an arbitrary magic number. Originally 128 but (mu) was also heavily reduced.
        double stop_probability = 1.0 / mu;

        if constexpr (debug3) {
            std::cout << "\t\t\tmu: " << mu << std::endl;
            std::cout << "\t\t\ttotal_markable_left: " << total_markable_left << std::endl;
        }

        for (size_t iter = 0; iter < k; ++iter) {
            size_t edges_left = successes_until_failure(stop_probability) + 1;
            if (edges_left > total_markable_left) {
                total_markable_left = 0;
            }
            else {
                total_markable_left -= edges_left;
            }

            if constexpr (debug3) {
                std::cout << "\t\t\tedges_left: " << edges_left << std::endl;
                std::cout << "\t\t\ttotal_markable_left: " << total_markable_left << std::endl;
            }

            if (iter == k - 1 && edges_left < mu) edges_left = mu; // At least that much on the last iteration.

            sg.new_search();
            sg.visit(x, { x, 0 });
            graph::internal_location dfs = { x, 0 };
            while (edges_left > 0) {
                if (dfs.i >= sg[dfs.v].size()) {
                    if (dfs.v == x) {
                        if constexpr (debug) {
                            std::cout << "\t\t\tRan out of edges" << std::endl;
                        }
                        break;
                    }

                    dfs = sg.internal_parent[dfs.v];
                    dfs.i++;
                    continue;
                }

                //if (--edges_left == 0) break; // Not in this version

                vertex w = sg[dfs.v][dfs.i];
                if (sg.is_visited[w]) {
                    dfs.i++;
                }
                else {
                    sg.visit(w, dfs);
                    dfs = { w, 0 };

                    // Count edges here.
                    if (!sg.is_counted[w]) {
                        sg.init_count(w);
                    }

                    if (sg.uncounted_edges[w] < edges_left) {
                        edges_left -= sg.uncounted_edges[w];
                        sg.uncounted_edges[w] = 0;
                    }
                    else {
                        sg.uncounted_edges[w] -= edges_left;
                        edges_left = 0;
                    }   
                }
            }

            if (edges_left > 0) {
                if constexpr (debug) {
                    std::cout << "\t\t\tstopped with edges left" << std::endl;
                }
                break; // We have a cut.
            }

            if (total_markable_left == 0) {
                if constexpr (debug4) {
                    std::cout << "\t\t\tmarked maximum markable quantity" << std::endl;
                }
                return false;
            }

            // On the last iteration, return no cut rather than reverse.
            if (iter == k - 1) {
                if constexpr (debug4) {
                    std::cout << "\t\t\tlast iteration had enough edges" << std::endl;
                }
                return false;
            }

            if constexpr (debug) {
                std::cout << "\t\t\tReversing from " << dfs.v << std::endl;
            }

            sg.reverse_source_to_vertex(dfs.v);
        }

        // If we reach this point, then we left the above loop with (edges_left > 0). There is a cut.
        auto reachable = std::unordered_set<vertex>(sg.visited_vertices.begin(), sg.visited_vertices.end());
        //auto reachable = get_reachable(sg, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k-1 outvertices unaccessible through chance.
        if constexpr (debug) {
            std::cout << "\t\t\treachable.size: " << reachable.size() << std::endl;
            std::cout << "\t\t\tMaxsize: " << sg.size() - k << std::endl;
        }
        if (reachable.size() < sg.size() - k) {
            cut = reachable;
            if constexpr (debug) {
                std::cout << "\t\t\tL: " << cut.size() << std::endl;
            }
            return true;
        }
        if constexpr (debug4) {
            std::cout << "\t\t\tLocalEC covered the whole graph." << std::endl;
        }
        return false;
    }
    


    // marking *without* the negative binomial distribution - mark exactly (nu) edges and choose one at random.
    bool local_edge_connectivity_marking2_degreecount(graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = false;
        const bool debug2 = false;
        const bool debug3 = false;

        const bool debug4 = false; // Reason for failure

        {
            size_t d_x = sg[x].size();
            if (mu <= d_x) {
                if constexpr (debug4) {
                    std::cout << "\t\t\ttoo low volume" << std::endl;
                }
                return false;
            }
            mu -= d_x; // The outgoing edges of x are *obviously* part of the cut, so let's only count other edges.
        }

        size_t max_stop = mu; // == 1 * mu / epsilon > 1 * mu * k

        for (size_t iter = 0; iter < k; ++iter) {
            size_t markable_left = max_stop;             // Always mark the full amount.
            size_t edges_left = random_int(1, max_stop); // This is the chosen edge to reverse from.
            vertex reversal_target = x;

            if constexpr (debug3) {
                std::cout << "\t\t\tedges_left: " << edges_left << std::endl;
            }

            sg.new_search();
            sg.visit(x, { x, 0 });
            graph::internal_location dfs = { x, 0 };
            while (markable_left > 0) {
                if (dfs.i >= sg[dfs.v].size()) {
                    if (dfs.v == x) {
                        if constexpr (debug) {
                            std::cout << "\t\t\tRan out of edges" << std::endl;
                        }
                        break;
                    }

                    dfs = sg.internal_parent[dfs.v];
                    dfs.i++;
                    continue;
                }

                vertex w = sg[dfs.v][dfs.i];
                if (sg.is_visited[w]) {
                    dfs.i++;
                }
                else {
                    sg.visit(w, dfs);
                    dfs = { w, 0 };

                    // Count edges here.
                    {
                        if (!sg.is_counted[w]) {
                            sg.init_count(w);
                        }

                        if (edges_left) {
                            if (sg.uncounted_edges[w] < edges_left) {
                                edges_left -= sg.uncounted_edges[w];
                            }
                            else {
                                edges_left = 0;
                                reversal_target = w;
                                if constexpr (debug3) {
                                    std::cout << "\t\t\tChoosing reversal edge: " << w << std::endl;
                                }
                            }
                        }

                        if (sg.uncounted_edges[w] < markable_left) {
                            markable_left -= sg.uncounted_edges[w];
                            sg.uncounted_edges[w] = 0;
                        }
                        else {
                            sg.uncounted_edges[w] -= markable_left;
                            markable_left = 0;
                        }
                    }
                }
            }

            if (markable_left > 0) {
                if constexpr (debug) {
                    std::cout << "\t\t\tstopped with edges left" << std::endl;
                }
                break; // We have a cut.
            }

            // On the last iteration, return no cut rather than reverse.
            if (iter == k - 1) {
                if constexpr (debug4) {
                    std::cout << "\t\t\tlast iteration had enough edges" << std::endl;
                }
                return false;
            }

            if constexpr (debug) {
                std::cout << "\t\t\tReversing from " << reversal_target << std::endl;
            }

            sg.reverse_source_to_vertex(reversal_target);
        }

        // If we reach this point, then we left the above loop with (edges_left > 0). There is a cut.
        auto reachable = std::unordered_set<vertex>(sg.visited_vertices.begin(), sg.visited_vertices.end());
        //auto reachable = get_reachable(sg, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k-1 outvertices unaccessible through chance.
        if constexpr (debug) {
            std::cout << "\t\t\treachable.size: " << reachable.size() << std::endl;
            std::cout << "\t\t\tMaxsize: " << sg.size() - k << std::endl;
        }
        if (reachable.size() < sg.size() - k) {
            cut = reachable;
            if constexpr (debug) {
                std::cout << "\t\t\tL: " << cut.size() << std::endl;
            }
            return true;
        }
        if constexpr (debug4) {
            std::cout << "\t\t\tLocalEC covered the whole graph." << std::endl;
        }
        return false;
    }

    bool local_edge_connectivity_minors(graph& sg, vertex x, size_t mu, size_t k, std::unordered_set<vertex>& cut) {
        const bool debug = false;

        size_t max_stop = 1 * mu * k + 1; // == 1 * mu / epsilon > 1 * mu * k

        for (size_t iter = 0; iter < k; ++iter) {
            size_t edges_left = random_int(1, max_stop) + 1;
            if (iter == k - 1) edges_left = max_stop + 1;

            sg.new_search();
            sg.visit(x, { x, 0 });
            graph::internal_location dfs = { x, 0 };
            while (true) {
                if (dfs.i >= sg[dfs.v].size()) {
                    if (dfs.v == x) {
                        if constexpr (debug) {
                            std::cout << "\t\t\tRan out of edges" << std::endl;
                        }
                        break;
                    }

                    dfs = sg.internal_parent[dfs.v];
                    dfs.i++;
                    continue;
                }

                if (dfs.v != x) {
                    if (--edges_left == 0) break; // If we reach this point we are visiting an edge.
                }

                vertex w = sg[dfs.v][dfs.i];
                if (sg.is_visited[w]) {
                    dfs.i++;
                }
                else {
                    sg.visit(w, dfs);
                    dfs = { w, 0 };
                }
            }

            if (edges_left > 0) {
                if constexpr (debug) {
                    std::cout << "\t\t\tstopped with edges left" << std::endl;
                }
                break; // Go to the k:th iteration. We have a cut.
            }

            // On the last iteration, return no cut rather than reverse.
            if (iter == k - 1) {
                if constexpr (debug) {
                    std::cout << "\t\t\tlast iteration had enough edges" << std::endl;
                }
                return false;
            }

            sg.reverse_source_to_vertex(dfs.v);
        }

        // If we reach this point, then we left the above loop with (edges_left > 0). There is a cut.
        auto reachable = std::unordered_set<vertex>(sg.visited_vertices.begin(), sg.visited_vertices.end());
        //auto reachable = get_reachable(sg, x);

        // At this point the graph is k-connected if and only if we visited the whole graph.
        // If this happens we actually called LocalEC with bad arguments.
        // Allow a slack of k vertices because we might have made k-1 outvertices unaccessible through chance.
        if constexpr (debug) {
            std::cout << "\t\t\treachable.size: " << reachable.size() << std::endl;
            std::cout << "\t\t\tMaxsize: " << sg.size() - k << std::endl;
        }
        if (reachable.size() < sg.size() - k) {
            cut = reachable;
            if constexpr (debug) {
                std::cout << "\t\t\tL: " << cut.size() << std::endl;
            }
            return true;
        }
        return false;
    }
    
}