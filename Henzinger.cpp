#include "pch.h"
#include "Henzinger.h"

#include <chrono>

#define COUTLINE(x) std::cout << x << std::endl


// The probability that s samples from a set of n are all in the same k elements.
// = k/n (k-i)/(n-i) ... (k-s+1)/(n-s+1)
// = [ k!/(k-i)! ] / [ n!/(n-i)! ]
double risk(size_t n, size_t k, size_t s) {
	if (s > k) return 0;

	double res = 1;
	for (size_t i = 0; i < s; i++) {
		res *= (k - i);
		res /= (n - i);
	}
	return res;
}

namespace henzinger {
	/* procedure initialize (plus constructing the split graph) */
	HenzingerVC::HenzingerVC(const adjacency_list& G, size_t s) {
		n = G.size();
		SG = adjacency_list(2 * n);
		for (size_t v = 0; v < n; v++) {
			SG[2 * v].push_back(2 * v + 1);
			for (size_t t : G[v])
				SG[2 * v + 1].push_back(2 * t);
		}
		current_edge_i = std::vector<size_t>(2 * n, 0);

		d = std::vector<size_t>(2 * n);
#if W_BY_DISTANCE_TYPE == 1
		W_by_distance = { std::vector<size_t>(), std::vector<size_t>() };
#elif W_BY_DISTANCE_TYPE == 2
		W_by_distance = { std::unordered_set<size_t>(), std::unordered_set<size_t>() };
#elif W_BY_DISTANCE_TYPE == 3
		wbd_i = std::vector<size_t>(2 * n);
		W_by_distance = { std::vector<size_t>(), std::vector<size_t>() };
#endif

		excess = std::vector<size_t>(2 * n);
		best_W = { 2 * ((s + 1) % n) };
		in_W = std::vector<bool>(2 * n, true);
		in_W[2 * s] = false; // Part of the implicitly contracted graph.

		
		// These vertices are also part of the implicitly contracted graph.
		for (size_t v : SG[2 * s + 1]) {
			// Do not add to W.
			in_W[v] = false;
			// Add to S.
			D[0].push_back(v);
			// Saturate each edge from v.
			for (size_t v2 : SG[v]) {
				SG[v2].push_back(v);
				excess[v2] += 1;
				if (in_W[v2] && excess[v2] == 1) // v2 becomes overflowing
					W_and_overflowing.push_back(v2);
			}
			SG[v].clear();
			excess[v] = 0; // This should *more* than empty the excess at v.
		}
				
		D[0].push_back(2 * s);
		d_min_D[0] = d_min_D_and_T[0] = 0;
		d_max_D[0] = 1;

		t = 2 * s + 1; // outvertex of s

		for (size_t v = 0; v < 2 * n; v++) {
			if (!in_W[v]) continue; // Skip vertices in the implicitly contracted source vertex (that we add to S in init) except t.

			d[v] = v % 2;
#if W_BY_DISTANCE_TYPE == 1
			W_by_distance[d[v]].push_back(v);
#elif W_BY_DISTANCE_TYPE == 2
			W_by_distance[d[v]].insert(v);
#elif  W_BY_DISTANCE_TYPE == 3
			wbd_i[v] = W_by_distance[d[v]].size();
			W_by_distance[d[v]].push_back(v);
#endif
		}
	}

	void HenzingerVC::new_sink() {
		auto& wbd = W_by_distance[d[t] - d_min_W];
#if W_BY_DISTANCE_TYPE == 1
		// Remove t from W_by_distance using the swap-pop idiom.
		auto it = std::find(wbd.begin(), wbd.end(), t);
		std::swap(*it, wbd.back());
		wbd.pop_back();
#elif W_BY_DISTANCE_TYPE == 2
		wbd.erase(t);
#elif W_BY_DISTANCE_TYPE == 3
		// Remove t from W_by_distance using the swap-pop idiom.
		wbd_i[wbd.back()] = wbd_i[t];
		wbd[wbd_i[t]] = wbd.back();
		wbd.pop_back();
#endif
		if (wbd.empty()) {
			if (d[t] == d_min_W) {
				W_by_distance.pop_front();
				d_min_W++;
			} else if (d[t] == d_min_W + W_by_distance.size() - 1) {
				W_by_distance.pop_back();
			}
		}
		in_W[t] = false;

		// Add t to S.
		D[0].push_back(t);
		if (d[t] < d_min_D[0])
			d_min_D[0] = d[t];
		if (d[t] < d_min_D_and_T[0] && in_T(t))
			d_min_D_and_T[0] = d[t];
		if (d[t] > d_max_D[0])
			d_max_D[0] = d[t];
		// Saturate each edge from t.
		for (size_t v : SG[t]) {
			SG[v].push_back(t);
			excess[v] += 1;
			if (in_W[v] && excess[v] == 1) // v becomes overflowing
				W_and_overflowing.push_back(v);
		}
		SG[t].clear();
		excess[t] = 0; // We are pushing more than the current excess out of t.

		// Merge D[gamma] if W does not contain a vertex in T.
		if (d_min_W % 2 == 1 && W_by_distance.size() == 1) { // W only contains vertices of a single (odd) distance. (using d-consecutiveness and parity properties)
			// Lazy join without moving vertices from W to D[gamma]
			d_min_W = d_min_D_and_T[gamma] + 1; // Every vertex is in the right W_by_distance partition.
			for (size_t v : W_by_distance[0]) {
				d[v] = d_min_W; // Every vertex has the right recorded distance.
				current_edge_i[v] = 0; // current edge is reset by join
			}

			// Make sure W_by_distance is large enough to merge D[gamma] into W.
			while (d_min_D[gamma] < d_min_W) {
				d_min_W -= 1;
				W_by_distance.push_front({});
			}

			while (d_max_D[gamma] >= d_min_W + W_by_distance.size()) {
				W_by_distance.push_back({});
			}

			// Merge D[gamma] into W.
			for (size_t v : D[gamma]) {
				in_W[v] = true;
#if W_BY_DISTANCE_TYPE == 1
				W_by_distance[d[v] - d_min_W].push_back(v);
#elif W_BY_DISTANCE_TYPE == 2
				W_by_distance[d[v] - d_min_W].insert(v);
#elif W_BY_DISTANCE_TYPE == 3
				auto& wbd = W_by_distance[d[v] - d_min_W];
				wbd_i[v] = wbd.size();
				wbd.push_back(v);
#endif
				if (excess[v] > 0)
					W_and_overflowing.push_back(v);
				current_edge_i[v] = 0;
			}
			// Remove D[gamma]
			D.pop_back();
			d_min_D.pop_back();
			d_max_D.pop_back();
			d_min_D_and_T.pop_back();
			gamma -= 1;
		}

		// Make sure d_min_W is even (the vertices with minimum distance in W are in T)
		if (d_min_W % 2 == 1) {
			// Ensure W_by_distance is the right size
			if (W_by_distance.size() == 2)
				W_by_distance.push_back({});
			// Increase the distance for all vertices in W_by_distance[0]
			for (size_t v : W_by_distance[0]) {
				d[v] += 2;
#if W_BY_DISTANCE_TYPE == 1
				W_by_distance[2].push_back(v);
#elif W_BY_DISTANCE_TYPE == 2
				W_by_distance[2].insert(v);
#elif W_BY_DISTANCE_TYPE == 3
				wbd_i[v] = W_by_distance[2].size();
				W_by_distance[2].push_back(v);
#endif
			}
			W_by_distance.pop_front(); d_min_W += 1;	
		}

		// Choose t as a vertex of W with d[t] = d_min_W
#if W_BY_DISTANCE_TYPE == 1 || W_BY_DISTANCE_TYPE == 3
		t = W_by_distance[0].front();
#elif W_BY_DISTANCE_TYPE == 2
		t = *(W_by_distance[0].begin());
#endif
	}

	// Push from v along the current edge of v.
	void HenzingerVC::push_from(size_t v) {
		const size_t w = SG[v][current_edge_i[v]];
		// Remove edge vw
		SG[v][current_edge_i[v]] = SG[v].back();
		SG[v].pop_back();
		// Add edge wv
		SG[w].push_back(v);
		// Increase excess of w and add it to the queue if it *became* overflowing.
		excess[w] += 1;
		if (excess[w] == 1)
			W_and_overflowing.push_back(w);
		// Reduce excess of v and add it back to the queue if it is still overflowing.
		excess[v] -= 1;
		if (excess[v] > 0)
			W_and_overflowing.push_back(v);
	}

	void HenzingerVC::relabel(size_t v) {
		//check("relabel");

		auto& wbd = W_by_distance[d[v] - d_min_W];
		size_t d_max_W = d_min_W + W_by_distance.size() - 1;
		if (wbd.size() == 1) {
			if (d[v] < d_max_W) {
				create_dormant(v);
			}
			else { // d[v] == d_max_W
				create_singleton_dormant_or_join(v);
			}
		} else {
			// Scan for an edge vu, u in W.
			size_t d_min_u = SIZE_MAX;
			for (const size_t& u : SG[v]) {
				if (in_W[u] && d_min_u > d[u]) {
					d_min_u = d[u];
				}
			}
			if (d_min_u == SIZE_MAX) { // No residual edge from v to W
				create_singleton_dormant_or_join(v);
			} else { // u exists (line 5)
				// Remove v from W_by_distance, its list is not empty afterwards.
#if W_BY_DISTANCE_TYPE == 1
				auto it = std::find(wbd.begin(), wbd.end(), v);
				std::swap(*it, wbd.back());
				wbd.pop_back();
#elif W_BY_DISTANCE_TYPE == 2
				wbd.erase(v);
#elif W_BY_DISTANCE_TYPE == 3
				wbd_i[wbd.back()] = wbd_i[v];
				wbd[wbd_i[v]] = wbd.back();
				wbd.pop_back();
#endif
				// Change d[v]
				d[v] = d_min_u + 1;
				// Ensure W_by_distance[d[v] - d_min_W] exists
				while (W_by_distance.size() <= d[v] - d_min_W)
					W_by_distance.push_back({});
				// Add v to W_by_distance
#if W_BY_DISTANCE_TYPE == 1
				W_by_distance[d[v] - d_min_W].push_back(v);
#elif W_BY_DISTANCE_TYPE == 2
				W_by_distance[d[v] - d_min_W].insert(v);
#elif W_BY_DISTANCE_TYPE == 3
				wbd_i[v] = W_by_distance[d[v] - d_min_W].size();
				W_by_distance[d[v] - d_min_W].push_back(v);
#endif
			}
		}
	}

	/* Create dormant set with all u such that d[u] >= d[v] */
	void HenzingerVC::create_dormant(size_t v) {
		D.push_back({});
		d_max_D.push_back({ d_min_W + W_by_distance.size() - 1 });
		d_min_D.push_back({ d[v] });
		d_min_D_and_T.push_back({ in_T(d[v]) ? d[v] : d[v] + 1 });
		gamma += 1;
		while (in_W[v]) {
			// Add all vertices u such that d[u] = d_max(W) to D[gamma]
			D[gamma].insert(D[gamma].end(), W_by_distance.back().begin(), W_by_distance.back().end());
			// Remove all such vertices from W
			for (size_t u : W_by_distance.back())
				in_W[u] = false;
			W_by_distance.pop_back();
		}
	}
	/* Create dormant set {v} if v is in T, otherwise join it. */
	void HenzingerVC::create_singleton_dormant_or_join(size_t v) {
		// Remove v from W.
		if (W_by_distance[d[v] - d_min_W].size() == 1) {
			// The only vertex at that distance in W (implies d[v] == d_max(W))
			W_by_distance.pop_back();
		} else {
			auto& wbd = W_by_distance[d[v] - d_min_W];
#if W_BY_DISTANCE_TYPE == 1
			auto it = std::find(wbd.begin(), wbd.end(), v);
			std::swap(*it, wbd.back());
			wbd.pop_back();
#elif W_BY_DISTANCE_TYPE == 2
			wbd.erase(v);
#elif W_BY_DISTANCE_TYPE == 3
			wbd_i[wbd.back()] = wbd_i[v];
			wbd[wbd_i[v]] = wbd.back();
			wbd.pop_back();
#endif
		}
		in_W[v] = false;

		if (in_T(v)) {
			// Add a dormant set {v}
			D.push_back({ v });
			d_min_D.push_back({ d[v] });
			d_max_D.push_back({ d[v] });
			d_min_D_and_T.push_back({ d[v] });
			gamma += 1;
		} else {
			// Join v to D[gamma]
			D[gamma].push_back(v);
			d[v] = d_min_D_and_T[gamma] + 1;
			if (d[v] > d_max_D[gamma])
				d_max_D[gamma] = d[v];
		}
	}

	void HenzingerVC::check(std::string id) {
		checkWBD_i(id);
		checkDconsecutiveness(id);
		checkDormancy(id);
		checkDValidity(id);
	}
	void HenzingerVC::checkWBD_i(std::string id) {
		for (size_t v = 0; v < 2 * n; v++) {
			if (!in_W[v])
				continue; // Only look at things in W.
			if (W_by_distance[d[v] - d_min_W][wbd_i[v]] != v)
				COUTLINE("wbd_i error: element " << wbd_i[v] << " at d=" << d[v] << " is " << W_by_distance[d[v] - d_min_W][wbd_i[v]] << ", not " << v << " at id: " << id);
		}
	}
	void HenzingerVC::checkDconsecutiveness(std::string id) {
		for (size_t i = 0; i < W_by_distance.size(); i++) {
			if (W_by_distance[i].empty())
				COUTLINE("W_by_distance[" << i << "] is empty. D consecutiveness might be compromised at id " << id);
		}
		// Skip checking D[0] = S
		for (size_t i = 1; i < D.size(); i++) {
			std::set<size_t> sizes;
			for (size_t v : D[i])
				sizes.insert(d[v]);
			if (sizes.size() == 1) continue; // Only one size is consecutive

			auto it = sizes.begin(); 
			auto it2 = sizes.begin(); it2++;
			for (; it2 != sizes.end(); it++, it2++) {
				if (*it + 1 != *it2)
					COUTLINE("D[" << i << "] is missing distance " << *it + 1 << ", compromising D consecutiveness at id " << id);
			}
		}
	}
	void HenzingerVC::checkDormancy(std::string id) {
		std::vector<size_t> current;
		std::unordered_set<size_t> allowed;

		for (size_t i = 0; i < D.size(); i++) {
			current = D[0];
			allowed.insert(current.begin(), current.end());
			for (size_t v : current) {
				for (size_t w : SG[v]) {
					if (allowed.find(w) == allowed.end())
						COUTLINE("Edge (" << v << "," << w << ") breaks dormancy at id " << id);
				}
			}
		}
	}

	void HenzingerVC::checkDValidity(std::string id) {
		std::vector<size_t> group(2 * n, 2 * n); // Extra bookkeeping that we don't need for the algorithm.
		for (size_t i = 0; i < D.size(); i++) {
			for (size_t v : D[i]) {
				group[v] = i;
			}
		}
		for (auto& wbd : W_by_distance) {
			for (size_t v : wbd) {
				group[v] = D.size();
			}
		}
		for (size_t v = 0; v < 2 * n; v++) {
			for (size_t w : SG[v]) {
				if (group[w] > group[v])
					COUTLINE("Edge (" << v << "," << w << ") breaks D-validity at id " << id);
			}
		}
	}

	std::unordered_set<size_t> min_vc_split(const adjacency_list& G, size_t s)
	{
		HenzingerVC vc(G, s);
		if (vc.W_by_distance.front().size() == 0) {
			// No vertices at d=0 -> No invertices in W -> All invertices contracted into s -> s adjacent to all vertices.
			// We should be choosing only vertices where this is not the case. If not possible, then the connectivity is k-1.
			std::unordered_set<size_t> cut;
			for (size_t v = 0; v < s; v++)
				cut.insert(v);
			for (size_t v = s+1; v < vc.n; v++)
				cut.insert(v);
			return cut;
		}
		while (true) { // exit by break
			vc.new_sink();

			while (!vc.W_and_overflowing.empty()) {
				size_t v = vc.W_and_overflowing.front(); vc.W_and_overflowing.pop_front();
				if (v == vc.t) continue; // t may have become overflowing in new_sink before it was chosen as t. Skip it here.
				if (!vc.in_W[v]) {
					// This is possible because we make (possibly overflowing) vertices dormant (in large numbers).
					continue;
				}
				// Try to find a residual edge vw with w in W and d[v] = d[w] + 1.
				// While not at the end of the list and the current adjacent vertex does not meet the requirements, increment current edge index

				const std::vector<size_t>& adj_v = vc.SG[v];
				size_t& i = vc.current_edge_i[v];
				while (i < adj_v.size() && (vc.d[adj_v[i]] + 1 != vc.d[v] || !vc.in_W[adj_v[i]])) {
					i++;
				}

				// If we found one (current edge is valid), push one unit of flow.
				if (vc.current_edge_i[v] < vc.SG[v].size()) {
					vc.push_from(v);
				}
				// If we did not find one, relabel.
				else {
					vc.current_edge_i[v] = 0;
					vc.relabel(v);
					if (vc.in_W[v])
						vc.W_and_overflowing.push_back(v);
				}
			}


			//if (vc.excess[vc.t] < vc.best_W_indegree) {
				/*
				 * Calculate the indegree of the set W in the *original* (not residual) split graph.
				 * There is no edge vu in the residual split graph that enters W from a dormant set.
				 * If the edge goes opposite to its original direction it was originally an edge into v (in W)
				 * If both endpoints are in W, then the edge is an edge into W.
				 */
				size_t W_indegree = 0;
				std::vector<size_t> W_neighbors;
				for (auto& wbd : vc.W_by_distance) {
					for (const size_t& v : wbd) {
						for (const size_t& u : vc.SG[v]) {
							if (vc.in_W[u]) // ignore u in W because then vu is an edge W -> W
								continue;
							bool orig_direction =
								(v % 2 == 0 && v + 1 == u) || // v is an invertex and u is its outvertex
								(v % 2 == 1 && v - 1 != u);   // v is an outvertex and v is not its invertex.
							if (orig_direction) // Ignore edges that go in their original direction because they go out from W, not in, in the original graph.
								continue;
							W_neighbors.push_back(u);
							W_indegree++;

							if (W_indegree > vc.best_W_indegree)
								break;
						}
						if (W_indegree > vc.best_W_indegree)
							break;
					}
					if (W_indegree > vc.best_W_indegree)
						break;
				}

				if (W_indegree < vc.best_W_indegree) { // If W is better than best_W
					vc.best_W_indegree = W_indegree;
					vc.best_W_neighbors = W_neighbors;

					// Clear best_W
					vc.best_W.clear();
					// Add the vertices of W to best_W
					for (auto& wbd : vc.W_by_distance) {
						vc.best_W.insert(vc.best_W.end(), wbd.begin(), wbd.end());
					}
				}

			if (vc.gamma > 0) continue; // If there are dormant sets other than S, then one definitely includes a vertex in T.

			bool exists_T_not_in_S = false;
			for (auto& wbd : vc.W_by_distance) {
				for (size_t v : wbd) {
					if (vc.in_T(v) && v != vc.t) {
						exists_T_not_in_S = true;
						break;
					}
				}
				if (exists_T_not_in_S) break;
			}
			if (!exists_T_not_in_S) break; // End the loop if T is a subset of (S and t)
		}

		std::unordered_set<size_t> cut;
		for (size_t v : vc.best_W_neighbors)
			cut.insert(v / 2);

		return cut;
	}

	std::unordered_set<size_t> min_vc_split_v2(const adjacency_list& G, size_t s) {
		HenzingerVC vc(G, s);
		if (vc.W_by_distance.front().size() == 0) {
			// No vertices at d=0 -> No invertices in W -> All invertices contracted into s -> s adjacent to all vertices.
			// We should be choosing only vertices where this is not the case. If not possible, then the connectivity is k-1.
			std::unordered_set<size_t> cut;
			for (size_t v = 0; v < s; v++)
				cut.insert(v);
			for (size_t v = s + 1; v < vc.n; v++)
				cut.insert(v);
			return cut;
		}
		while (true) { // exit by break
			if (vc.t % 2 == 1) {
				// First iteration.
				vc.new_sink();
			} else {
				bool was_not_overflowing = (vc.excess[vc.t + 1] == 0);
				vc.excess[vc.t + 1] = vc.SG[vc.t].size();
				if (vc.in_W[vc.t + 1] && was_not_overflowing && vc.excess[vc.t + 1] != 0) {
					vc.W_and_overflowing.push_back(vc.t + 1);
				}
				vc.new_sink();
			}

			while (!vc.W_and_overflowing.empty()) {
				size_t v = vc.W_and_overflowing.front(); vc.W_and_overflowing.pop_front();
				if (v == vc.t) continue; // t may have become overflowing in new_sink before it was chosen as t. Skip it here.
				if (!vc.in_W[v]) {
					// This is possible because we make (possibly overflowing) vertices dormant in bulk.
					continue;
				}
				// Try to find a residual edge vw with w in W and d[v] = d[w] + 1.
				// While not at the end of the list and the current adjacent vertex does not meet the requirements, increment current edge index

				const std::vector<size_t>& adj_v = vc.SG[v];
				size_t& i = vc.current_edge_i[v];
				while (i < adj_v.size() && (vc.d[adj_v[i]] + 1 != vc.d[v] || !vc.in_W[adj_v[i]])) {
					i++;
				}

				// If we found one (current edge is valid), push one unit of flow.
				if (vc.current_edge_i[v] < vc.SG[v].size()) {
					vc.push_from(v);
				}
				// If we did not find one, relabel.
				else {
					vc.current_edge_i[v] = 0;
					vc.relabel(v);
					if (vc.in_W[v])
						vc.W_and_overflowing.push_back(v);
				}
			}

			/*
			 * Calculate the indegree of the set W in the *original* (not residual) split graph.
			 * There is no edge vu in the residual split graph that enters W from a dormant set.
			 * If the edge goes opposite to its original direction it was originally an edge into v (in W)
			 * If both endpoints are in W, then the edge is an edge into W.
			 */
			size_t W_indegree = 0;
			std::vector<size_t> W_neighbors;
			for (auto& wbd : vc.W_by_distance) {
				for (const size_t& v : wbd) {
					for (const size_t& u : vc.SG[v]) {
						if (vc.in_W[u]) // ignore u in W because then vu is an edge W -> W
							continue;
						bool orig_direction =
							(v % 2 == 0 && v + 1 == u) || // v is an invertex and u is its outvertex
							(v % 2 == 1 && v - 1 != u);   // v is an outvertex and v is not its invertex.
						if (orig_direction) // Ignore edges that go in their original direction because they go out from W, not in, in the original graph.
							continue;
						W_neighbors.push_back(u);
						W_indegree++;

						if (W_indegree > vc.best_W_indegree)
							break;
					}
					if (W_indegree > vc.best_W_indegree)
						break;
				}
				if (W_indegree > vc.best_W_indegree)
					break;
			}

			if (W_indegree < vc.best_W_indegree) { // If W is better than best_W
				vc.best_W_indegree = W_indegree;
				vc.best_W_neighbors = W_neighbors;

				// Clear best_W
				vc.best_W.clear();
				// Add the vertices of W to best_W
				for (auto& wbd : vc.W_by_distance) {
					vc.best_W.insert(vc.best_W.end(), wbd.begin(), wbd.end());
				}
			}

			if (vc.gamma > 0) continue; // If there are dormant sets other than S, then one definitely includes a vertex in T.

			bool exists_T_not_in_S = false;
			for (auto& wbd : vc.W_by_distance) {
				for (size_t v : wbd) {
					if (vc.in_T(v) && v != vc.t) {
						exists_T_not_in_S = true;
						break;
					}
				}
				if (exists_T_not_in_S) break;
			}
			if (!exists_T_not_in_S) break; // End the loop if T is a subset of (S and t)
		}

		std::unordered_set<size_t> cut;
		for (size_t v : vc.best_W_neighbors)
			cut.insert(v / 2);

		return cut;
	}

	std::unordered_set<size_t> min_vc_split_pair(const adjacency_list& G, size_t s, size_t t)
	{
		HenzingerVC vc(G, s);
		vc.new_sink();
		vc.t = 2 * t; // override the sink chosen by the algorithm. 

		while (!vc.W_and_overflowing.empty()) {
			size_t v = vc.W_and_overflowing.front(); vc.W_and_overflowing.pop_front();
			if (v == vc.t) continue; // t may have become overflowing in new_sink before it was chosen as t. Skip it here.
			if (!vc.in_W[v]) {
				// This is possible because we make (possibly overflowing) vertices dormant in bulk.
				continue;
			}
			// Try to find a residual edge vw with w in W and d[v] = d[w] + 1.
			// While not at the end of the list and the current adjacent vertex does not meet the requirements, increment current edge index
			while (vc.current_edge_i[v] < vc.SG[v].size() && (!vc.in_W[vc.SG[v][vc.current_edge_i[v]]] || vc.d[v] != vc.d[vc.SG[v][vc.current_edge_i[v]]] + 1)) {
				vc.current_edge_i[v]++;
			}
			// If we found one (current edge is valid), push one unit of flow.
			if (vc.current_edge_i[v] < vc.SG[v].size()) {
				vc.push_from(v);
			}
			// If we did not find one, relabel.
			else {
				vc.current_edge_i[v] = 0;
				vc.relabel(v);
				if (vc.in_W[v])
					vc.W_and_overflowing.push_back(v);
			}
		}

		/*
		 * Calculate the indegree of the set W in the *original* (not residual) split graph.
		 * There is no edge vu in the residual split graph that enters W from a dormant set.
		 * If the edge goes opposite to its original direction it was originally an edge into v (in W)
		 * If both endpoints are in W, then the edge is an edge into W.
		 */
		std::unordered_set<size_t> cut;
		for (auto& wbd : vc.W_by_distance) {
			for (const size_t& v : wbd) {
				for (const size_t& u : vc.SG[v]) {
					if (vc.in_W[u]) // ignore u in W because then vu is an edge W -> W
						continue;
					bool orig_direction =
						(v % 2 == 0 && v + 1 == u) || // v is an invertex and u is its outvertex
						(v % 2 == 1 && v - 1 != u);   // v is an outvertex and v is not its invertex.
					if (orig_direction) // Ignore edges that go in their original direction because they go out from W, not in, in the original graph.
						continue;
					cut.insert(u / 2);
				}
			}
		}

		return cut;
	}

	std::unordered_set<size_t> vc_undirected_linear(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		sparse::labelling labelling = sparse::nagamochi_ibraki_labelling(G);
		size_t i = 0;
		size_t k = G.size() - 1;
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		while (i <= 2 * k) {
			i += 1;

			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end())
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			adjacency_list FG = sparse::nagamochi_ibraki_adj_list(labelling, i);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto dt = (t_1 - t_0);
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(dt).count();

			size_t m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();
			
			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() < std::min(i, k)) {
				min_cut = cut;
				k = cut.size();
				x = s;
			}
		}

		return min_cut;
	}

	std::unordered_set<size_t> vc_directed_linear(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		adjacency_list RG(G.size()); // Reverse graph.
		for (size_t s = 0; s < G.size(); s++) {
			for (size_t t : G[s])
				RG[t].push_back(s);
		}

		size_t i = 0;
		size_t k = G.size() - 1;
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		size_t m = 0;
		for (std::vector<size_t> ls : G)
			m += ls.size();

		while (i <= k) {
			i += 1;

			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end())
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(G, s);
			std::unordered_set<size_t> rcut = min_vc_split(RG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (rcut.size() < cut.size())
				std::swap(cut, rcut);

			if (cut.size() >= i) continue; // Not viable cut in current FG.

			if (cut.size() < std::min(i, k)) {
				min_cut = cut;
				k = cut.size();
				x = s;
			}
		}

		return min_cut;
	}

	std::unordered_set<size_t> vc_undirected_linear_random(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		sparse::labelling labelling = sparse::nagamochi_ibraki_labelling(G);
		size_t i = 0;
		size_t k = G.size() - 1;
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		while (i <= 2 * k) {
			i += 1;

			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end())
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			adjacency_list FG = sparse::nagamochi_ibraki_adj_list(labelling, i);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			size_t m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() >= i) continue; // Not viable cut in current FG.

			if (cut.size() < std::min(i, k)) {
				min_cut = cut;
				k = cut.size();
				x = s;
			}

			// At least one viable cut has been found.
			if (min_cut.size() > 1) {
				double r = risk(G.size(), k, i - k);
				//COUTLINE("Estimated error rate: " << r);
				if (r < 0.5) break;
			}
		}

		return min_cut;
	}

	std::unordered_set<size_t> vc_undirected_linear_unsparsified(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		size_t i = 0;
		size_t k = G.size() - 1;
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		size_t m = 0;
		for (std::vector<size_t> ls : G)
			m += ls.size();

		while (i <= k) {
			i += 1;

			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end())
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(G, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() < k) {
				min_cut = cut;
				k = cut.size();
				x = s;
			}
		}

		return min_cut;
	}

	std::unordered_set<size_t> vc_undirected_linear_unsparsified_random(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		size_t i = 0;
		size_t k = G.size() - 1;
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		size_t m = 0;
		for (std::vector<size_t> ls : G)
			m += ls.size();

		while (i <= k) {
			i += 1;

			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end())
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(G, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() < k) {
				min_cut = cut;
				k = cut.size();
				x = s;
			}

			if (min_cut.size() > 0) {
				double r = risk(G.size(), k, i);
				//COUTLINE("Estimated error rate: " << r);
				if (r < 0.5) break;
			}

		}

		return min_cut;
	}

	std::unordered_set<size_t> vc_undirected_doubling_random(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		sparse::labelling labelling = sparse::nagamochi_ibraki_labelling(G);
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		size_t s = labelling.lastVertex;
		starting_points.insert(x);
		size_t k;
		for (k = 1; true; k *= 2) {
			adjacency_list FG = sparse::nagamochi_ibraki_adj_list(labelling, k);
			size_t m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ k, cut.size(), m, t_1 - t_0 });

			if (cut.size() < k) {
				min_cut = cut;
				k = cut.size() + 1;
				x = s;
				break;
			}
		}

		adjacency_list FG = sparse::nagamochi_ibraki_adj_list(labelling, k);
		size_t n = FG.size();
		size_t m = 0;
		for (std::vector<size_t> ls : FG)
			m += ls.size();
		size_t limit = (size_t)std::ceil(1 / std::log2((double)n / k));
		for (size_t i = 0; i < limit; i++) {
			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end())
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() < min_cut.size()) {
				min_cut = cut;
				x = s;
			}
		}

		return min_cut;
	}

	std::unordered_set<size_t> vc_undirected_doubling2_random(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		sparse::labelling labelling = sparse::nagamochi_ibraki_labelling(G);
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		adjacency_list FG;
		size_t n = 0, m = 0;
		size_t k;

		for (k = 1; true; k *= 2) {
			FG = sparse::nagamochi_ibraki_adj_list(labelling, k);
			m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();

			// Find a new *random* starting point. (This method is kind of bad unless k << n
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end() && starting_points.size() < n)
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ k, cut.size(), m, t_1 - t_0 });

			if (cut.size() < k) {
				min_cut = cut;
				k = cut.size() + 1;
				break;
			}
		}

		n = G.size();
		size_t limit = (size_t)std::ceil(1 / std::log2((double)n / k));
		if (limit > n) limit = n;

		if (1 < limit) {
			FG = sparse::nagamochi_ibraki_adj_list(labelling, k);
			m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();
		}

		starting_points.clear();

		for (size_t i = 1; i < limit; i++) {
			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end() && starting_points.size() < n)
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() < min_cut.size()) {
				min_cut = cut;
				k = cut.size() + 1;
				limit = (size_t)std::ceil(1 / std::log2((double)n / k));
				if (limit > n) limit = n;
			}
				
		}

		return min_cut;
	}

	std::unordered_set<size_t> vc2_undirected_doubling2_random(const adjacency_list& G, size_t& x, std::vector<henzinger_subresult>& subresults) {
		sparse::labelling labelling = sparse::nagamochi_ibraki_labelling(G);
		std::unordered_set<size_t> min_cut;
		std::unordered_set<size_t> starting_points;

		adjacency_list FG;
		size_t n = 0, m = 0;
		size_t k;

		for (k = 1; true; k *= 2) {
			FG = sparse::nagamochi_ibraki_adj_list(labelling, k);
			m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();

			// Find a new *random* starting point. (This method is kind of bad unless k << n
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end() && starting_points.size() < n)
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split_v2(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ k, cut.size(), m, t_1 - t_0 });

			if (cut.size() < k) {
				min_cut = cut;
				k = cut.size() + 1;
				break;
			}
		}

		n = G.size();
		size_t limit = (size_t)std::ceil(1 / std::log2((double)n / k));
		if (limit > n) limit = n;

		if (1 < limit) {
			FG = sparse::nagamochi_ibraki_adj_list(labelling, k);
			m = 0;
			for (std::vector<size_t> ls : FG)
				m += ls.size();
		}

		starting_points.clear();

		for (size_t i = 1; i < limit; i++) {
			// Find a new *random* starting point.
			size_t s = random_int(0, G.size() - 1);
			while (starting_points.find(s) != starting_points.end() && starting_points.size() < n)
				s = random_int(0, G.size() - 1);
			starting_points.insert(s);

			auto t_0 = std::chrono::high_resolution_clock::now();
			std::unordered_set<size_t> cut = min_vc_split_v2(FG, s);
			auto t_1 = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count();

			subresults.push_back({ i, k, m, t_1 - t_0 });

			if (cut.size() < min_cut.size()) {
				min_cut = cut;
				k = cut.size() + 1;
				limit = (size_t)std::ceil(1 / std::log2((double)n / k));
				if (limit > n) limit = n;
			}

		}

		return min_cut;
	}
}

#undef COUTLINE
