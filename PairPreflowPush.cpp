#include "pch.h"

#include "PairPreflowPush.h"
#include "Util.h"

namespace preflow {
	preflow_helper::preflow_helper(const adjacency_list& G, size_t source, size_t target) {
		n = G.size();
		s = 2 * source + 1;
		t = 2 * target;
		SG = adjacency_list(2 * n);
		for (size_t v = 0; v < n; v++) {
			SG[2 * v].push_back(2 * v + 1);
			for (size_t u : G[v])
				SG[2 * v + 1].push_back(2 * u);
		}

		current_edge_i = std::vector<size_t>(2 * n);
		excess = std::vector<size_t>(2 * n);
		d = std::vector<size_t>(2 * n);
		d[s] = 2 * n;

		active_by_distance = { {} };
		for (size_t v = 0; v < 2 * n; v++) {
			if (v != s)
				active_by_distance[0].push_back(v);
		}

		for (size_t u : SG[s]) {
			SG[u].push_back(s);
			excess[u]++;
			if (excess[u] == 1) {
#if Q_version == 1
				Q1.push_back(u);
#else if Q_version == 2
				Q2.push(u, d);
#endif
			}
				
		}
		SG[s].clear();

		//bfs_init_d_undirected(G, source, target);

		run(G);
	}

	// TODO intialise d to be the actual distance to [t]
	void preflow_helper::bfs_init_d_undirected(const adjacency_list& G, size_t source, size_t target) {
		std::vector<bool> visited(n); visited[target] = visited[source] = true;
		std::deque<size_t> Q = { target };
		while (!Q.empty()) {
			size_t v = Q.front(); Q.pop_front();
			for (size_t w : G[v]) {
				if (visited[w]) continue;
				visited[w] = true;
				Q.push_back(w);

				d[2 * w + 1] = d[2 * v] + 1;
				d[2 * w] = d[2 * v] + 2;
			}
		}
	}

	void preflow_helper::run(const adjacency_list& G) {
#if Q_version == 1
		while (!Q1.empty()) {
			size_t v = Q1.front(); Q1.pop_front();
#else if Q_version == 2
		while (!Q2.empty()) {
			size_t v = Q2.top(); Q2.pop();
#endif
			if (v == t || d[v] == n)
				continue;
			while (excess[v] > 0 && d[v] < 2 * n) {
				if (current_edge_i[v] == SG[v].size()) {
					//check_abd("r1");
					relabel(v);
					//check_abd("r2");
					current_edge_i[v] = 0;
				} else {
					if (d[v] == d[SG[v][current_edge_i[v]]] + 1) {
						push_current(v);
					}
					else
						current_edge_i[v]++;
					
				}
			}
		}
	}

	void preflow_helper::push_current(size_t v) {
		size_t w = SG[v][current_edge_i[v]];

		std::swap(SG[v][current_edge_i[v]], SG[v].back()); // Move current edge vw to the back of the adjacency list
		SG[v].pop_back(); // Remove residual edge vw
		SG[w].push_back(v); // Add residual edge wv.

		excess[v]--; // move excess.
		excess[w]++;

		// Push w to Q if w became overflowing.
		if (excess[w] == 1) {
#if Q_version == 1
			Q1.push_back(w);
#else if Q_version == 2
			Q2.push(w, d);
#endif
		}
			
	}
	void preflow_helper::relabel(size_t v) {
		if (active_by_distance[d[v]].size() == 1) {
			// About to disconnect all vertices at or above this distance from the sink. Mark them dormant.
			size_t d_v = d[v];
			for (size_t d_w = d_v; d_w < active_by_distance.size(); d_w++) {
				for (size_t w : active_by_distance[d_w]) {
					d[w] = 2 * n;
				}
			}
			active_by_distance.resize(d_v);

			return; // No need for the rest of the relabel function.
		}
		
		size_t d_min_neighbors = SIZE_MAX;
		for (size_t i = 0; i < SG[v].size(); i++) {
			const size_t& d_i = d[SG[v][i]];
			if (d_i < d_min_neighbors) {
				d_min_neighbors = d_i;
				current_edge_i[v] = i; // Doesn't seem to be mentioned anywhere but I think it's valid. Anything before the i:th edge won't be admissible.
			}
		}
		auto it = std::find(active_by_distance[d[v]].begin(), active_by_distance[d[v]].end(), v);
		*it = active_by_distance[d[v]].back();
		active_by_distance[d[v]].pop_back();
		//active_by_distance[d[v]].erase(std::remove(active_by_distance[d[v]].begin(), active_by_distance[d[v]].end(), v)); // Is this ok?
		d[v] = d_min_neighbors + 1; // neighbors in the residual graph
		if (d[v] >= active_by_distance.size()) {
			active_by_distance.push_back({});
		}
		if(d[v] < 2 * n)
			active_by_distance[d[v]].push_back(v);
	}

	size_t preflow_helper::k() {
		return excess[t];
	}

	std::unordered_set<size_t> preflow_helper::get_vertex_separator(const adjacency_list& G) {
		std::unordered_set<size_t> S;
		// Make a reverse of the current residual split graph.
		adjacency_list SG_rev(2 * n);
		for (size_t v = 0; v < 2 * n; v++) {
			for (size_t w : SG[v]) {
				SG_rev[w].push_back(v);
			}
		}
		// Find the vertices reachable from the sink in SG_rev.
		std::vector<bool> visited(2 * n, false);
		std::deque<size_t> bfs_queue = { t }; visited[t] = true;
		while (!bfs_queue.empty()) {
			size_t v = bfs_queue.front(); bfs_queue.pop_front();
			for (size_t w : SG_rev[v]) {
				if (visited[w]) continue; // already added to bfs queue
				visited[w] = true;
				bfs_queue.push_back(w);
			}
		}

		size_t visited_count = std::count(visited.begin(), visited.end(), true);
		//std::cout << visited_count << " vertices can reach the sink..." << std::endl;

		// Find cut edges.
		for (size_t v = 0; v < n; v++) {
			size_t v_in = 2 * v;
			size_t v_out = v_in + 1;

			if (visited[v_out]) {
				if (!visited[v_in]) {
					//std::cout << "case a: " << v << std::endl;
					S.insert(v); // (v_in, v_out) is a cut edge.
				}
			}
			else {
				for (size_t w : G[v]) {
					if (visited[2 * w]) {
						//std::cout << "case b: " << v << std::endl;
						S.insert(v); // vw is a cut edge.
						break;
					}

				}
			}
		}

		return S;
	}

	bool pair_vertex_connectivity(const adjacency_list& G, size_t s, size_t t, std::unordered_set<size_t>& min_cut, size_t k) {
		preflow_helper h(G, s, t);
		bool found_cut = false;
		if (h.k() < k) {
			auto cut = h.get_vertex_separator(G);
			if (cut.size() != h.k())
				std::cout << "preflow helper claimed k = " << h.k() << " but gave a vertex separator of size " << cut.size() << std::endl;
			min_cut = cut;
			found_cut = true;
		}
		return found_cut;
	}


	void preflow_helper::check_abd(const std::string& id) {
		bool all_ok = true;

		std::vector<bool> ok(2 * n, false); // active and in the correct place.

		for (size_t d_v = 0; d_v < active_by_distance.size(); d_v++) {
			for (size_t v : active_by_distance[d_v]) {
				if (d[v] != d_v) {
					std::cout << "d[" << v << "] = " << d[v] << ", " << v << " in abd[" << d_v << "]" << " id: " << id << std::endl;
					all_ok = false;
				}
				if (ok[v]){
					std::cout << v << " duplicate in abd" << " id: " << id << std::endl;
					all_ok = false;
				}
				ok[v] = true;
			}
		}
		for (size_t v = 0; v < 2 * n; v++) {
			if (d[v] < n && !ok[v]) {
				std::cout << "d[" << v << "] = " << d[v] << " but is not in abd" << " id: " << id << std::endl;
				all_ok = false;
			}
		}

		if (!all_ok) {
			std::cout << active_by_distance.size() << " active distances." << std::endl;
			std::cout << "active_by_distance sizes:";
			for (auto& x : active_by_distance)
				std::cout << " " << x.size();
			std::cout << std::endl;
			size_t sum = 0;
			for (auto& x : active_by_distance)
				sum += x.size();
			std::cout << "sum: " << sum << std::endl;
		}
	}


}