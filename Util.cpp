#include "pch.h"
#include "Util.h"
#include "random.h"
#include <unordered_set>

/*
 * Get the neighbors of set S in graph g.
 * The set of vertices t such that there is a vertex s in S and an edge s -> t in g but t is not in S.
 */
std::unordered_set<versioned_graph::vertex> versioned_graph::get_neighbors(versioned_graph::graph& g, const std::unordered_set<versioned_graph::vertex>& S) {
    std::unordered_set<vertex> N;
    for (vertex s : S) {
        auto end = g.end_of(s);
        for (auto eit = g.begin_of(s); eit != end; ++eit) {
            vertex t = eit.get_target();
            if (S.find(t) == S.end()) {
                N.insert(t);
            }
        }
    }
    return N;
}
std::unordered_set<resettable_graph::vertex> resettable_graph::get_neighbors(resettable_graph::graph& g, const std::unordered_set<resettable_graph::vertex>& S) {
    std::unordered_set<vertex> N;
    for (vertex s : S) {
        for (vertex t : g[s]) {
            if (S.find(t) == S.end()) {
                N.insert(t);
            }
        }
    }
    return N;
}
std::unordered_set<versioned_graph::vertex> versioned_graph::get_neighbors(const std::vector<std::vector<vertex>>& g, const std::unordered_set<vertex>& S) {
    std::unordered_set<vertex> N;
    for (vertex s : S) {
        for(vertex t : g[s]) {
            if (S.find(t) == S.end()) {
                N.insert(t);
            }
        }
    }
    return N;
}

/*
 * Get the set of vertices y in g such that there is a path from x to y in g. 
 */
std::unordered_set<versioned_graph::vertex> versioned_graph::get_reachable(versioned_graph::graph& g, vertex x) {
    std::unordered_set<vertex> visited;
    visited.insert(x);
    for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
        visited.insert(eit.get_target());
    }
    return visited;
}
std::unordered_set<resettable_graph::vertex> resettable_graph::get_reachable(resettable_graph::graph& g, vertex x) {
    std::unordered_set<vertex> visited = { x };
    
    g.new_search();
    g.visit(x, { x, 0 });
    std::vector<vertex> Q = { x };
    while (!Q.empty()) {
        vertex v = Q.back(); Q.pop_back();
        for (index i = 0; i < g[v].size(); i++) {
            const vertex& w = g[v][i];
            if (g.is_visited[w]) continue;
            g.visit(v, w, i);
            visited.insert(w);
            Q.push_back(w);
        }
    }

    return visited;
}
std::unordered_set<versioned_graph::vertex> versioned_graph::get_reachable(const std::vector<std::vector<vertex>>& g, vertex x) {
    std::vector<bool> is_visited(g.size(), false); is_visited[x] = true;
    std::unordered_set<vertex> visited; visited.insert(x);

    std::vector<vertex> Q = { x };
    while (!Q.empty()) {
        vertex v = Q.back(); Q.pop_back();
        for (vertex w : g[v]) {
            if (is_visited[w]) continue;
            is_visited[w] = true;
            visited.insert(w);
            Q.push_back(w);
        }
    }

    return visited;
}

/*
 * Get the set of vertices y in g such that there is a path from x to y in g.
 */
size_t versioned_graph::get_reachable_volume(graph& g, vertex x) {
    size_t vol = 0;
    for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
        vol++;
    }
    return vol;
}
size_t resettable_graph::get_reachable_volume(resettable_graph::graph& g, vertex x) {
    size_t vol = g[x].size();

    std::vector<bool> is_visited(g.size(), false); is_visited[x] = true;

    std::vector<vertex> Q = { x };
    while (!Q.empty()) {
        vertex v = Q.back(); Q.pop_back();
        for (vertex w : g[v]) {
            if (is_visited[w]) continue;
            is_visited[w] = true;
            vol += g[w].size();
            Q.push_back(w);
        }
    }

    return vol;
}
/*
 * Get the set of vertices y in g such that there is a path from x to y in g.
 */
size_t versioned_graph::get_vertex_cut_volume(graph& g, vertex x, std::unordered_set<vertex> cut) {
    size_t vol = 0;
    for (auto eit = g.new_dfs(x); eit.is_edge(); g.advance_dfs(eit)) {
        if (cut.find(eit.get_source()) == cut.end()) {
            vol++;
        } else {
            g.backtrack_dfs(eit); // Outgoing edge from a cut edge. Backtrack without counting the edge.
        }
    }
    return vol;
}

versioned_graph::edge_source_sampler::edge_source_sampler(const graph& g) {
    for (vertex s = 0; s < g.size_vertices(); s++) {
        auto end = g.end_of(s);
        for (auto eit = g.begin_of(s); eit != end; ++eit) {
            data.push_back(s); // Save source vertex s once for every outgoing edge from it.
        }
    }
}
versioned_graph::edge_source_sampler::edge_source_sampler(const std::vector<std::vector<vertex>>& g) {
    for (vertex s = 0; s < g.size(); s++) {
        for (size_t i = g[s].size(); i > 0; i--) {
            data.push_back(s);
        }
    }
}

versioned_graph::vertex versioned_graph::edge_source_sampler::get_vertex() {
    if (i == data.size()) reset(); // Restart if all edges have been sampled.
    size_t r = random_int(i, data.size() - 1); // Sample one of the unsampled indices.
    std::swap(data[i], data[r]); // Move that vertex to the front of the unsampled interval.
    return data[i++]; // Return that vertex and increment i to make it part of the sampled part of 'data' vector
}

size_t versioned_graph::edge_source_sampler::n_edges()
{
    return data.size();
}

void versioned_graph::edge_source_sampler::reset() {
    i = 0;
}


