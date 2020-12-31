#include "pch.h"
#include "Graphgen.h"
#include "Util.h"
#include "random.h"
#include <vector>
#include <algorithm>
#include <numeric>

namespace versioned_graph {
	graph make_complete_graph(size_t n) {
		graph g;
		for (vertex s = 0; s < n; ++s) {
			for (vertex t = 0; t < n; ++t) {
				if (s == t)
					continue;
				g.add_edge_unsafe(s, t);
			}
			g.add_vertex();
		}
		return g;
	}
	graph make_randomised_complete_graph(size_t n) {
		std::vector<std::vector<vertex>> vec;
		for (vertex s = 0; s < n; ++s) {
			vec.push_back({});
			for (vertex t = 0; t < n; ++t) {
				if (s == t)
					continue;
				vec[s].push_back(t);
			}
		}
		for (std::vector<vertex>& v : vec) {
			random_shuffle(v);
		}
		graph g(vec);
		return g;
	}
	graph make_complete_LSR_graph(size_t n_1, size_t n_2, size_t n_3) {
		graph g;
		size_t N_1 = n_1;
		size_t N_2 = n_1 + n_2;
		size_t N_3 = n_1 + n_2 + n_3;
		vertex s = 0;
		// L -> {L, S}
		for (; s < N_1; ++s) {
			for (vertex t = 0; t < N_2; ++t) {
				if (s == t)
					continue;
				g.add_edge_unsafe(s, t);
			}
			g.add_vertex();
		}
		// S -> {L, S, R}
		for (; s < N_2; ++s) {
			for (vertex t = 0; t < N_3; ++t) {
				if (s == t)
					continue;
				g.add_edge_unsafe(s, t);
			}
			g.add_vertex();
		}
		// R -> {S, R}
		for (; s < N_3; ++s) {
			for (vertex t = N_1; t < N_3; ++t) {
				if (s == t)
					continue;
				g.add_edge_unsafe(s, t);
			}
			g.add_vertex();
		}
		return g;
	}

	std::vector<std::vector<vertex>> make_sparsified_complete_LSR_adj_list(size_t n_1, size_t n_2, size_t n_3) {
		size_t N_1 = n_1;
		size_t N_2 = n_1 + n_2;
		size_t N_3 = n_1 + n_2 + n_3;
		size_t n = N_3;

		std::vector<std::vector<vertex>> adjacency_list(n);

		size_t k = n_2 + 1;
		size_t missing_connections_L = k;
		size_t missing_connections_R = k;;

		std::unordered_set<vertex> fully_connected_vertices;

		while (missing_connections_L > 0) {
			vertex v = random_int(0, N_2 - 1);
			if (fully_connected_vertices.find(v) != fully_connected_vertices.end())
				continue;
			missing_connections_L -= 1;
			if (v >= N_1)
				missing_connections_R -= 1;
			fully_connected_vertices.insert(v);
		}

		while (missing_connections_R > 0) {
			vertex v = random_int(N_2, N_3 - 1);
			if (fully_connected_vertices.find(v) != fully_connected_vertices.end())
				continue;
			missing_connections_R -= 1;
			fully_connected_vertices.insert(v);
		}

		std::vector<vertex> S(fully_connected_vertices.begin(), fully_connected_vertices.end());
		std::sort(S.begin(), S.end());
		for (vertex s : S) {
			auto it = S.begin();
			for (vertex t = 0; t < n; ++t) {
				if (it != S.end() && *it == t) { // skip t in S
					++it;
					continue;
				}
				if (s < N_1 && t >= N_2) // Skip if s in L and t in R.
					continue;
				if (t < N_1 && s >= N_2) // Skip if t in L and s in R.
					continue;

				adjacency_list[s].push_back(t);
				adjacency_list[t].push_back(s);
			}
			for (vertex t : S) {
				if (s <= t) // Symmetry breaking
					continue;
				if (s < N_1 && t >= N_2) // Skip if s in L and t in R.
					continue;
				if (t < N_1 && s >= N_2) // Skip if t in L and s in R.
					continue;
				adjacency_list[s].push_back(t);
				adjacency_list[t].push_back(s);
			}
		}
		return adjacency_list;
	}

	graph make_sparsified_complete_LSR_graph(size_t n_1, size_t n_2, size_t n_3) {
		return graph(make_sparsified_complete_LSR_adj_list(n_1, n_2, n_3));
	}

	graph make_exact_degree_LSR_multigraph(size_t n_1, size_t n_2, size_t n_3, size_t d) {
		return graph(make_exact_degree_LSR_multigraph_adj_list(n_1, n_2, n_3, d));
	}

	// Degree 2d
	adj_list make_undirected_min_degree_LSR_graph_adj_list(size_t n_1, size_t n_2, size_t n_3, size_t d) {
		size_t N_1 = n_1;
		size_t N_2 = n_1 + n_2;
		size_t N_3 = n_1 + n_2 + n_3;

		std::vector<vertex> vertices(N_3);
		std::iota(std::begin(vertices), std::end(vertices), 0);
		random_shuffle(vertices);

		//std::cout << "LS:";
		//for (size_t i = 0; i < N_2; i++)
		//	std::cout << " " << vertices[i];
		//std::cout << std::endl;

		std::vector<std::vector<vertex>> adjacency_list(N_3);

		// d edges per vertex in L
		for (vertex k = 0; k < N_1; ++k) {
			vertex s = vertices[k];
			for (size_t i = 0; i < d; ++i) {
				size_t j = random_int(0, N_2 - 2);
				if (vertices[j] == s) j = N_2 - 1;
				vertex t = vertices[j];

				adjacency_list[t].push_back(s);
				adjacency_list[s].push_back(t);
			}
		}
		// + d edges per vertex in S
		for (vertex k = N_1; k < N_2; ++k) {
			vertex s = vertices[k];
			for (size_t i = 0; i < d; ++i) {
				size_t j = random_int(0, N_3 - 2);
				if (vertices[j] == s) j = N_3 - 1;
				vertex t = vertices[j];

				adjacency_list[t].push_back(s);
				adjacency_list[s].push_back(t);
			}
		}
		// + d edges per vertex in R
		for (vertex k = N_2; k < N_3; ++k) {
			vertex s = vertices[k];
			for (size_t i = 0; i < d; ++i) {
				size_t j = random_int(N_1 + 1, N_3 - 1);
				if (vertices[j] == s) j = N_1;
				vertex t = vertices[j];

				adjacency_list[t].push_back(s);
				adjacency_list[s].push_back(t);
			}
		}

		for (auto& vec : adjacency_list) {
			std::sort(vec.begin(), vec.end());
			vec.erase(std::unique(vec.begin(), vec.end()), vec.end()); // Henzinger-Rao-Gabow can't handle the multigraph.
			random_shuffle(vec); // re-randomize
		}

		return adjacency_list;
	}

	adj_list make_undirected_FG_LSR_graph_adj_list(size_t n_1, size_t n_2, size_t n_3, size_t k) {
		size_t N_2 = n_1 + n_2;
		size_t N_3 = N_2 + n_3;

		struct e { size_t v; size_t w; };
		std::vector<e> candidate_edges;

		std::vector<vertex> vertices(N_3);
		std::iota(std::begin(vertices), std::end(vertices), 0);
		random_shuffle(vertices);

		for (size_t i = 0; i < n_1; i++) {
			vertex v = vertices[i];
			for (size_t j = i + 1; j < N_2; j++) {
				vertex w = vertices[j];
				candidate_edges.push_back({ v, w });
			}
		}
		for (size_t i = n_1; i < N_2; i++) {
			vertex v = vertices[i];
			for (size_t j = i + 1; j < N_3; j++) {
				vertex w = vertices[j];
				candidate_edges.push_back({ v, w });
			}
		}
		for (size_t i = N_2; i < N_3; i++) {
			vertex v = vertices[i];
			for (size_t j = i + 1; j < N_3; j++) {
				vertex w = vertices[j];
				candidate_edges.push_back({ v, w });
			}
		}

		std::vector<std::vector<vertex>> adjacency_list(N_3);

		std::vector<union_find> uf;
		for (size_t i = 0; i < k; i++) uf.push_back(union_find(N_3));

		random_shuffle(candidate_edges);

		for (e& edge : candidate_edges) {
			size_t v = edge.v;
			size_t w = edge.w;
			for (size_t i = 0; i < k; i++) {
				if (uf[i].find(v) == uf[i].find(w)) continue;

				adjacency_list[v].push_back(w);
				adjacency_list[w].push_back(v);
				uf[i].merge(v, w);
				break;
			}
		}

		return adjacency_list;
	}


	adj_list make_exact_degree_LSR_multigraph_adj_list(size_t n_1, size_t n_2, size_t n_3, size_t d) {
		size_t N_1 = n_1;
		size_t N_2 = n_1 + n_2;
		size_t N_3 = n_1 + n_2 + n_3;

		std::vector<vertex> vertices(N_3);
		std::iota(std::begin(vertices), std::end(vertices), 0);
		random_shuffle(vertices);

		std::vector<vertex> L, S, R;
		for (vertex s = 0; s < N_1; ++s) {
			for (size_t i = 0; i < d; ++i)
				L.push_back(vertices[s]);
		}
		for (vertex s = N_1; s < N_2; ++s) {
			for (size_t i = 0; i < d; ++i)
				S.push_back(vertices[s]);
		}
		for (vertex s = N_2; s < N_3; ++s) {
			for (size_t i = 0; i < d; ++i)
				R.push_back(vertices[s]);
		}

		std::vector<std::vector<vertex>> adjacency_list(N_3);

		// Some guaranteed S -> L
		//for (vertex s = N_1; s < N_2; ++s) {
		//	size_t j = random_int(0, L.size());
		//	vertex t = L.at(j);
		//	adjacency_list[s].push_back(t);
		//	L.at(j) = L.back();
		//	L.pop_back();
		//}

		// L -> {L, S}
		for (vertex s = 0; s < N_1; ++s) {
			for (size_t i = 0; i < d; ++i) {
				size_t j = random_int(0, L.size() + S.size() - 1);
				if (j < L.size()) {
					vertex t = L.at(j);
					adjacency_list[s].push_back(t);
					L.at(j) = L.back();
					L.pop_back();
					continue;
				}
				j -= L.size();

				adjacency_list[s].push_back(S.at(j));
				S.at(j) = S.back();
				S.pop_back();
			}
		}
		// R -> {S, R}
		for (vertex s = N_2; s < N_3; ++s) {
			for (size_t i = 0; i < d; ++i) {
				size_t j = random_int(0, S.size() + R.size() - 1);
				if (j < S.size()) {
					vertex t = S.at(j);
					adjacency_list[s].push_back(t);
					S.at(j) = S.back();
					S.pop_back();
					continue;
				}
				j -= S.size();

				adjacency_list[s].push_back(R.at(j));
				R.at(j) = R.back();
				R.pop_back();
			}
		}
		// S -> {L, S, R} (the rest of S)
		for (vertex s = N_1; s < N_2; ++s) {
			for (size_t i = 0; i < d - 1; ++i) {
				size_t j = random_int(0, L.size() + S.size() + R.size() - 1);
				if (j < L.size()) {
					vertex t = L.at(j);
					adjacency_list[s].push_back(t);
					L.at(j) = L.back();
					L.pop_back();
					continue;
				}
				j -= L.size();

				if (j < S.size()) {
					vertex t = S.at(j);
					adjacency_list[s].push_back(t);
					S.at(j) = S.back();
					S.pop_back();
					continue;
				}
				j -= S.size();

				vertex t = R.at(j);
				adjacency_list[s].push_back(t);
				R.at(j) = R.back();
				R.pop_back();
			}
		}


		return adjacency_list;
	}

	adj_list make_randomised_complete_LSR_graph_adj_list(size_t n_1, size_t n_2, size_t n_3) {
		std::vector<vertex> vertices(n_1 + n_2 + n_3);
		std::iota(std::begin(vertices), std::end(vertices), 0);
		random_shuffle(vertices);
		// TODO finish randomising the vertices in L, S and R.

		std::vector<std::vector<vertex>> vec;
		size_t N_1 = n_1;
		size_t N_2 = n_1 + n_2;
		size_t N_3 = n_1 + n_2 + n_3;
		// L -> {L, S}
		for (vertex s = 0; s < N_1; ++s) {
			vec.push_back({});
			for (vertex t = 0; t < N_2; ++t) {
				if (s == t)
					continue;
				vec[s].push_back(t);
			}
		}
		// S -> {L, S, R}
		for (vertex s = N_1; s < N_2; ++s) {
			vec.push_back({});
			for (vertex t = 0; t < N_3; ++t) {
				if (s == t)
					continue;
				vec[s].push_back(t);
			}
		}
		// R -> {S, R}
		for (vertex s = N_2; s < N_3; ++s) {
			vec.push_back({});
			for (vertex t = N_1; t < N_3; ++t) {
				if (s == t)
					continue;
				vec[s].push_back(t);
			}
		}
		for (std::vector<vertex>& v : vec) {
			std::shuffle(v.begin(), v.end(), std::default_random_engine());
		}
		return vec;
	}

	graph make_randomised_complete_LSR_graph(size_t n_1, size_t n_2, size_t n_3) {
		return graph(make_randomised_complete_LSR_graph_adj_list(n_1, n_2, n_3));
	}

	std::vector<std::vector<vertex>> make_randomised_LSR_erdos_renyi(size_t n_1, size_t n_2, size_t n_3, double p_in, double p_out) {
		size_t N_2 = n_1 + n_2;
		size_t N_3 = N_2 + n_3;

		std::vector<vertex> vertices(N_3);
		std::iota(std::begin(vertices), std::end(vertices), 0);
		random_shuffle(vertices);
		std::vector<vertex> L, S, R;
		L.insert(L.begin(), vertices.begin(), vertices.begin() + n_1);
		S.insert(S.begin(), vertices.begin() + n_1, vertices.begin() + N_2);
		R.insert(R.begin(), vertices.begin() + N_2, vertices.begin() + N_3);

		std::bernoulli_distribution bernoulli_in(p_in);
		std::bernoulli_distribution bernoulli_out(p_out);

		std::vector<std::vector<vertex>> adj;

		for (vertex v : L) {
			for (vertex w : L) {
				if (bernoulli_in(random_engine)) {
					adj[v].push_back(w);
				}
			}
			for (vertex w : S) {
				if (bernoulli_out(random_engine)) {
					adj[v].push_back(w);
				}
			}
		}
		for (vertex v : R) {
			for (vertex w : R) {
				if (bernoulli_in(random_engine)) {
					adj[v].push_back(w);
				}
			}
			for (vertex w : S) {
				if (bernoulli_out(random_engine)) {
					adj[v].push_back(w);
				}
			}
		}
		for (vertex v : S) {
			for (vertex w : L) {
				if (bernoulli_out(random_engine)) {
					adj[v].push_back(w);
				}
			}
			for (vertex w : S) {
				if (bernoulli_in(random_engine)) {
					adj[v].push_back(w);
				}
			}
			for (vertex w : R) {
				if (bernoulli_out(random_engine)) {
					adj[v].push_back(w);
				}
			}
		}

		for (std::vector<vertex>& vec : adj) {
			random_shuffle(vec);
		}

		return adj;
	}

	std::vector<std::vector<vertex>> make_randomised_undirected_LSR_erdos_renyi(size_t n_1, size_t n_2, size_t n_3, double p_in, double p_out) {
		size_t N_2 = n_1 + n_2;
		size_t N_3 = N_2 + n_3;

		std::vector<vertex> vertices(N_3);
		std::iota(vertices.begin(), vertices.end(), 0);
		random_shuffle(vertices);

		std::vector<vertex> L, S, R;
		L.insert(L.end(), vertices.begin(),       vertices.begin() + n_1);
		S.insert(S.end(), vertices.begin() + n_1, vertices.begin() + N_2);
		R.insert(R.end(), vertices.begin() + N_2, vertices.begin() + N_3);

		std::bernoulli_distribution bernoulli_in(p_in);
		std::bernoulli_distribution bernoulli_out(p_out);

		std::vector<std::vector<vertex>> adj(N_3);

		for (vertex v : L) {
			for (vertex w : L) {
				if (v < w && bernoulli_in(random_engine)) {
					adj[v].push_back(w);
					adj[w].push_back(v);
				}
			}
			for (vertex w : S) {
				if (bernoulli_out(random_engine)) {
					adj[v].push_back(w);
					adj[w].push_back(v);
				}
			}
		}
		for (vertex v : R) {
			for (vertex w : R) {
				if (v < w && bernoulli_in(random_engine)) {
					adj[v].push_back(w);
					adj[w].push_back(v);
				}
			}
			for (vertex w : S) {
				if (bernoulli_out(random_engine)) {
					adj[v].push_back(w);
					adj[w].push_back(v);
				}
			}
		}
		for (vertex v : S) {
			for (vertex w : S) {
				if (v < w && bernoulli_in(random_engine)) {
					adj[v].push_back(w);
					adj[w].push_back(v);
				}
			}
		}

		for (std::vector<vertex>& vec : adj) {
			random_shuffle(vec);
			//std::sort(vec.begin(), vec.end());
		}

		/*
		std::cout << "Print debugging from Graphgen.cpp" << std::endl;
		std::cout << "L:";
		for (vertex v : L)
			std::cout << " " << v;
		std::cout << std::endl;
		std::cout << "S:";
		for (vertex v : S)
			std::cout << " " << v;
		std::cout << std::endl;
		*/

		/*
		std::cout << "R:";
		for (vertex v : R)
			std::cout << " " << v;
		std::cout << std::endl;

		size_t d_min = SIZE_MAX;
		vertex v_min = 0;;
		for (vertex v = 0; v < N_3; v++) {
			if (adj[v].size() < d_min) {
				d_min = adj[v].size();
				v_min = v;
			}
		}
		std::cout << v_min << " has min degree " << d_min << std::endl;

		std::cout << "N(0):";
		for (vertex v : adj[0])
			std::cout << " " << v;
		std::cout << std::endl;

		std::cout << "N(" << R[0] << "):";
		for (vertex v : adj[R[0]])
			std::cout << " " << v;
		std::cout << std::endl;
		*/

		return adj;
	}

	std::vector<std::vector<vertex>> make_split_graph_adj_list(const std::vector<std::vector<vertex>>& g) {
		size_t n = g.size();
		std::vector<std::vector<vertex>> g_split(2 * n);
		for (size_t i = 0; i < n; i++) {
			g_split[2 * i].push_back(2 * i + 1);
			for(size_t t : g[i])
				g_split[2 * i + 1].push_back(2 * t);
		}
		return g_split;
	}
	graph make_split_graph(const graph& orig) {
		graph split_g;
		size_t n = orig.size_vertices();
		for (vertex s = 0; s < n; ++s) {
			vertex s_split_1 = 2 * s;     // in-vertex
			vertex s_split_2 = 2 * s + 1; // out-vertex

			split_g.add_edge_unsafe(s_split_1, s_split_2);
			split_g.add_vertex();

			auto end = orig.end_of(s);
			for (auto eit = orig.begin_of(s); eit != end; ++eit) {
				vertex t_split = 2 * eit.get_target();
				split_g.add_edge_unsafe(s_split_2, t_split);
			}
			split_g.add_vertex();
		}
		return split_g;
	}
	graph make_reverse_graph(const graph& orig) {
		size_t n = orig.size_vertices();
		std::vector<std::vector<vertex>> adjacency_list(n);

		for (vertex s = 0; s < n; ++s) {
			auto end = orig.end_of(s);
			for (auto eit = orig.begin_of(s); eit != end; ++eit) {
				adjacency_list[eit.get_target()].push_back(eit.get_source());
			}
		}

		return graph(adjacency_list);
	}

	/*
	 * Makes a version of orig such that for each pair of vertices x, y,
	 * x and y are k-connected iff they are k-connected in orig.
	 * WARNING: ASSUMES THAT orig IS BIDIRECTIONAL.
	 */
	graph make_sparse_graph(const graph& orig, const size_t k) {
		if (k == 0) return graph();

		size_t n = orig.size_vertices();
		std::vector<std::vector<vertex>> adjacency_list(n);

		std::vector<size_t> r(n, 0);
		std::vector<bool> vertex_scanned(n, false);
		// pending_buckets[i] is the set of vertices v with r(v) == i. Not removed from lower buckets when added to higher ones.
		std::vector<std::vector<vertex>> pending_buckets(k + 1, std::vector<vertex>());
		for (vertex v = 0; v < n; ++v)
			pending_buckets[0].push_back(v);

		std::vector<size_t> prev_bucket(k + 1, 0);
		size_t current_bucket = 0;

		while (!pending_buckets[current_bucket].empty()) {	// While there are vertices
			size_t i = random_int(0, pending_buckets[current_bucket].size() - 1);
			vertex s = pending_buckets[current_bucket][i];	// Choose an unscanned node s with the highest r(s)
			pending_buckets[current_bucket][i] = pending_buckets[current_bucket].back();
			pending_buckets[current_bucket].pop_back();

			//if (vertex_scanned[s]) continue;				// Skip if s is already scanned
			vertex_scanned[s] = true;						// Mark s scanned

			auto end = orig.end_of(s);
			for (auto eit = orig.begin_of(s); eit != end; ++eit) {	// For each edge incident to s
				vertex t = eit.get_target();

				//if (s == t) continue;					// Skip selfloops. (redundant with the next line when we mark s scanned early)
				if (vertex_scanned[t]) continue;		// Skip already scanned edges (target vertex scanned).
				if (r[t] == k) continue;				// Skip if this target already has maximum r(s). No edges will be added to the graph.

				adjacency_list[s].push_back(t);			// Add edge {st} to E_{r(t) + 1} which is part of the sparse graph
				adjacency_list[t].push_back(s);			// Add edge {ts} to E_{r(t) + 1}
				if (r[s] == r[t]) r[s]++;				// Increase r(s) if equal to r(t).
				r[t]++;									// Increase r(t).
				pending_buckets[r[t]].push_back(t);		// Increase r(t).
			}

			if (current_bucket != k && !pending_buckets[current_bucket + 1].empty()) {
				// Move to the next bucket if it is nonempty.
				if (pending_buckets[current_bucket].empty()) {
					prev_bucket[current_bucket + 1] = prev_bucket[current_bucket];
				}
				else {
					prev_bucket[current_bucket + 1] = current_bucket;
				}
				current_bucket += 1;
			}
			else {
				// Otherwise, go backward if the current bucket is also empty.
				if (pending_buckets[current_bucket].empty()) {
					current_bucket = prev_bucket[current_bucket];
				}
			}
		}

		for (auto& bucket : pending_buckets) if (!bucket.empty()) throw 0;

		return graph(adjacency_list);
	}
} // namespace versioned_graph

namespace resettable_graph {
	graph make_split_graph(const graph& orig) {
		graph split_g;
		size_t n = orig.size();
		for (vertex s = 0; s < n; ++s) {
			vertex s_split_1 = 2 * s;     // in-vertex
			vertex s_split_2 = 2 * s + 1; // out-vertex

			split_g.add_edge(s_split_1, s_split_2);

			for (const vertex& t : orig[s]) {
				split_g.add_edge(s_split_2, 2 * t);
			}
		}
		return split_g;
	}
} // namespace resettable_graph