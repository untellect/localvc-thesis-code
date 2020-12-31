#include "pch.h"
#include "MaxFlow.h"

#include <deque>
#include <list>

/*
 * Runs the ford-fulkerson algorithm to find the maximum flow (unsigned integer) between s and t.
 * Flow is represented through reversed edges.
 * The graph is left unreset so that the related edge cut can be found by finding the set S of reachable vertices, resetting and then finding N(S) (which is the cut).
 * Edge capacities other than 1 are not supported (except by adding extra edges to the graph)
 */
size_t maxflow::ford_fulkerson(graph& g, vertex s, vertex t, size_t k) {
	size_t flow = 0;
	for (graph::edge_iterator eit = g.new_dfs(s); eit.is_edge(); g.advance_dfs(eit)) {
		if (eit.get_target() == t) {
			++flow;
			g.reverse_path(eit);
			if (flow >= k)
				return flow;
			eit = g.new_dfs(s); // Restart the loop. Slight abuse of the for syntax.
			if (!eit.is_edge()) // Check the loop condition before advancing.
				break;
		}
	}
	return flow;
}

size_t maxflow::edmonds_karp(adjacency_list& RG, vertex s, vertex t, size_t k) {
	size_t flow = 0;

	struct edge { // RG[v][i]
		vertex v;
		size_t i;
	};

	std::vector<bool> visited(RG.size()); visited[s] = true;
	std::vector<edge> parent(RG.size()); parent[s] = { SIZE_MAX, 0 };
	std::deque<size_t> Q; Q.push_back(s);
	while (!Q.empty()) {
		vertex v = Q.front(); Q.pop_front();
		//if (v == t) {
		//	edge e = parent[v];
		//	while (e.v != SIZE_MAX) {
		//		RG[RG[e.v][e.i]].push_back(e.v);

		//		RG[e.v][e.i] = RG[e.v].back();
		//		RG[e.v].pop_back();

		//		e = parent[e.v];
		//	}
		//	std::fill(visited.begin(), visited.end(), false); visited[s] = true;
		//	Q = { s };
		//
		//	flow++;
		//	continue;
		//}


		for (size_t i = 0; i < RG[v].size(); i++) {
			vertex w = RG[v][i];
			if (visited[w]) continue;

			if (w == t) {
				edge e = { v, i };
				while (e.v != SIZE_MAX) {
					RG[RG[e.v][e.i]].push_back(e.v);

					RG[e.v][e.i] = RG[e.v].back();
					RG[e.v].pop_back();

					e = parent[e.v];
				}
				std::fill(visited.begin(), visited.end(), false); visited[s] = true;
				Q = { s };

				flow++;
				break;
			}


			visited[w] = true;
			parent[w] = { v, i };
			Q.push_back(w);
		}
	}

	return flow;
}

size_t maxflow::ford_fulkerson_adj(adjacency_list& RG, vertex s, vertex t, size_t k) {
	size_t flow = 0;

	struct Q_element {
		size_t i = 0;
		vertex v;
		Q_element(vertex v) : v(v) {};
	};

	std::vector<bool> visited(RG.size());
	std::deque<Q_element> Q;
	Q.push_back(s);

	// Try to push flow by DFS.
	while (!Q.empty()) {
		const vertex& v = Q.back().v;
		size_t& i = Q.back().i;

		if (v == t) {
			// Found flow.
			flow++;
			// Reverse edges along the DFS stack.

			while (Q.size() > 1) {
				Q.pop_back();
				const size_t& v = Q.back().v;
				size_t& i = Q.back().i;

				const size_t& w = RG[v][i];
				RG[w].push_back(v);

				RG[v][i] = RG[v].back();
				RG[v].pop_back();
			}

			if (flow == k)
				return flow;

			// Reset DFS
			std::fill(visited.begin(), visited.end(), false);
			visited[s] = true;
			Q.back().i = 0;
		}
		else {
			//auto it = std::find_if(RG[v].begin() + i, RG[v].end(), [visited](size_t v) { return !visited[v]; });
			//i = (it - RG[v].begin());

			// This loop is the hot spot by a large margin but alternative versions don't seem to change much.
			while (i < RG[v].size() && visited[RG[v][i]])
				i++;

			if (i < RG[v].size()) {
				// Advance DFS
				vertex w = RG[v][i];
				Q.push_back(w);
				visited[w] = true;
			}
			else {
				// Backtrack
				Q.pop_back();
				if (!Q.empty())
					Q.back().i++;
			}
		}
	}

	return flow;
}

size_t maxflow::ford_fulkerson_dfs_adj(adjacency_list& RG, vertex s, vertex t, size_t k) {
	size_t flow = 0;

	std::vector<vertex> prev(RG.size());
	prev[s] = s;
	std::vector<size_t> prev_i(RG.size());
	std::vector<bool> visited(RG.size());
	std::vector<vertex> Q;

	for (bool found_cut = true; found_cut; found_cut = false) {
		std::fill(visited.begin(), visited.end(), false);
		visited[s] = true;
		Q.clear();
		Q.push_back(s);

		while (!Q.empty()) {
			vertex v = Q.back(); Q.pop_back();
			for (size_t i = 0; i < RG[v].size(); i++) {
				vertex w = RG[v][i];
				if (visited[w])
					continue;

				// Reverse the path.
				if (w == t) {
					flow++;

					// RG[w].push_back(v); // Add (t -> v) // This is actually pointless because it's an outgoing edge from t.

					vertex p = prev[v];
					while (p != v) {
						RG[v][i] = p; // Replace (v -> w) with (v -> p)
						//w = v;
						i = prev_i[v];
						v = p;
						p = prev[v];
					}
					RG[v][i] = RG[v].back(); // Remove (s -> w)
					RG[v].pop_back();
				}

				visited[w] = true;
				prev[w] = v;
				prev_i[w] = i;
				Q.push_back(w);
			}
		}
	}

	return flow;
}

/*
std::unordered_set<size_t> maxflow::ford_fulkerson_list(adjacency_list& RG, vertex s, vertex t, size_t k) {
	size_t flow = 0;

	std::vector<std::list<size_t>> RG2(RG.size());
	for (size_t i = 0; i < RG.size(); i++)
		RG2[i].insert(RG2[i].end(), RG[i].begin(), RG[i].end());

	struct Q_element {
		std::list<size_t>::iterator it;
		vertex v;
		Q_element(vertex v, std::vector<std::list<size_t>>& RG2) : v(v), it(RG2[v].begin()) {};
	};

	std::vector<bool> visited(RG.size());
	std::deque<Q_element> Q;
	Q.emplace_back(s, RG2);

	// Try to push flow by DFS.
	while (!Q.empty()) {
		const vertex& v = Q.back().v;
		std::list<size_t>::iterator& it = Q.back().it;

		if (v == t) {
			// Found flow.
			flow++;
			// Reverse edges along the DFS stack.

			while (Q.size() > 1) {
				Q.pop_back();
				const size_t& v = Q.back().v;
				std::list<size_t>::iterator it = Q.back().it;

				RG2[*it].push_back(v);

				RG2[v].erase(it);
			}

			if (flow == k)
				return {};

			// Reset DFS
			std::fill(visited.begin(), visited.end(), false);
			visited[s] = true;
			Q.back().it = RG2[Q.back().v].begin();
		}
		else {
			while (it != RG2[v].end() && visited[*it])
				it++;
			if (it != RG2[v].end()) {
				// Advance DFS
				Q.emplace_back(*it, RG2);
				visited[*it] = true;
			}
			else {
				// Backtrack
				Q.pop_back();
				if (!Q.empty())
					Q.back().it++;
			}
		}
	}

	std::unordered_set<size_t> S;
	for (size_t v = 0; v < RG2.size(); v++) {
		if (!visited[v]) continue;
		for (size_t w : RG[v]) {
			if (!visited[w]) S.insert(w / 2);
		}
	}

	return S;
}
*/

// NOT DONE
size_t maxflow::dinics(adjacency_list& RG, vertex s, vertex t, size_t k) {
	std::vector<size_t> level(RG.size());
	std::vector<size_t> current_i(RG.size());
	std::deque<vertex> Q;

	size_t flow = 0;

	while (flow < k) {
		// Construct level graph by BFS
		for (size_t& l : level) l = SIZE_MAX;
		level[s] = 0; Q.clear(); Q.push_back(s);
		while (!Q.empty()) {
			size_t v = Q.front(); Q.pop_front();
			for (size_t w : RG[v]) {
				if (level[w] == SIZE_MAX) {
					level[w] = level[v] + 1;
					Q.push_back(w);
					if (w == t) {
						Q.clear(); // When 't' has been labelled we are done.
					}
				}
			}
		}
		// Stop if level graph has no path no t.
		if (level[t] == SIZE_MAX)
			break;
		// Push flow by DFS until level graph is blocked.
		Q.clear(); Q.push_back(s);
		for (size_t& i : current_i)
			i = 0;
		while (!Q.empty()) {
			vertex v = Q.back();
			size_t level_w = level[v] + 1;

			while (current_i[v] < RG[v].size() && level[RG[v][current_i[v]]] != level_w) {
				current_i[v]++;
			}

			// Out of edges -> backtrack DFS.
			if (current_i[v] >= RG[v].size()) {
				Q.pop_back();
				if (!Q.empty())
					current_i[Q.back()]++;
				continue;
			}

			vertex w = RG[v][current_i[v]];
			// Not target vertex -> advance DFS.
			if (w != t) {
				Q.push_back(w);
				continue;
			}

			// Found target vertex -> push flow from s to t.
			flow++;

			//RG[w].push_back(v); // Add (t -> v) // This is actually pointless because it's an outgoing edge from t.

			while (Q.size() > 1) {
				Q.pop_back(); // Pop v off the queue.
				vertex p = Q.back();

				RG[v][current_i[v]] = p; // Replace (v -> w) with (v -> p)

				vertex v = p;            // Backtrack.
			}
			RG[v][current_i[v]] = RG[v].back(); // Remove (s -> w) through swap-and-pop.
			RG[v].pop_back();

			// Q = {s}, flow has been pushed, continue.
		}
	}
	return flow;
}