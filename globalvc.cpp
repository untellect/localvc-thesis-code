#include "pch.h"

#include "MaxFlow.h"
#include "localvc.h"
#include "localvc2.h"
#include "Graphgen.h"
#include "Util.h"
#ifdef WIN32
#include "Optim.h"
#endif
#include "Sparse.h"
#include "PairPreflowPush.h"

#include <algorithm>

//#define PRINTFOUNDCUT

#include <chrono>

namespace localvc {
	/*
	 * Repeatedly samples pairs of vertices and used ford fulkerson to check if they have at least k edge-disjoint paths.
	 * Finds a cut with both sides at least a vertices
	 */
	bool vertex_connectivity_balanced_vertex_sampling(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		graph g(adj_list);
		graph g_split = make_split_graph(g);

		size_t n = g.size_vertices();
		for (size_t i = 0; i < repetitions; i++) { // n/a times, rounded up.
			vertex x_out = 2 * random_int(0, n - 1) + 1;
			vertex y_in = 2 * random_int(0, n - 1);
			g_split.reset();
			size_t k_xy = maxflow::ford_fulkerson(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(g, L);
				cut = S;
				return true;
			}
		}
		return false;
	}

	bool vertex_connectivity_balanced_edge_sampling(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		graph g(adj_list);
		graph g_split = make_split_graph(g);
		edge_source_sampler sampler(adj_list);

		size_t n = g.size_vertices();
		for (size_t i = 0; i < repetitions; i++) { // n/a times, rounded up.
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			g_split.reset();
			size_t k_xy = maxflow::ford_fulkerson(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(g, L);
				cut = S;
				return true;
			}
		}
		return false;
	}

	bool vertex_connectivity_balanced_edge_sampling_adj_list(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		edge_source_sampler sampler(adj_list);

		size_t n = adj_list.size();
		for (size_t i = 0; i < repetitions; i++) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(adj_list, L);
				cut = S;
				return true;
			}
		}
		return false;
	}

	bool vertex_connectivity_balanced_finish_edge_sampling_adj_list(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		edge_source_sampler sampler(adj_list);

		bool found_cut = false;

		size_t n = adj_list.size();
		for (size_t i = 0; i < repetitions; i++) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(adj_list, L);
				cut = S;
				found_cut = true;
				k = k_xy;
			}
		}
		return found_cut;
	}

	bool vertex_connectivity_balanced_dinic_edge_sampling_adj_list(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		edge_source_sampler sampler(adj_list);

		size_t n = adj_list.size();
		for (size_t i = 0; i < repetitions; i++) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::dinics(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(adj_list, L);
				cut = S;
				return true;
			}
		}
		return false;
	}

	bool vertex_connectivity_balanced_vertex_sampling_adj_list(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		edge_source_sampler sampler(adj_list);

		size_t n = adj_list.size();
		for (size_t i = 0; i < repetitions; i++) {
			vertex x_out = 2 * random_int(0, n - 1) + 1;
			vertex y_in = 2 * random_int(0, n - 1);
			std::vector<std::vector<vertex>> g_split = make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(adj_list, L);
				cut = S;
				return true;
			}
		}
		return false;
	}

	/*
	 * Repeatedly samples pairs of vertices and used ford fulkerson to check if they have at least k edge-disjoint paths.
	 * Finds a cut with both sides at least a vertices
	 */
	bool nonstop_vertex_connectivity_balanced_vertex_sampling(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		graph g(adj_list);
		graph g_split = make_split_graph(g);

		bool found_cut = false;

		size_t n = g.size_vertices();
		for (size_t i = 0; i < repetitions; i++) { // n/a times, rounded up.
			vertex x_out = 2 * random_int(0, n - 1) + 1;
			vertex y_in = 2 * random_int(0, n - 1);
			g_split.reset();
			size_t k_xy = maxflow::ford_fulkerson(g_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				cut = get_neighbors(g, L);
				found_cut = true;
			}
		}
		return found_cut;
	}

	bool vertex_connectivity_push_relabel_edge_sampling(const adj_list& adj_list, size_t k, size_t repetitions, std::unordered_set<vertex>& cut) {
		edge_source_sampler sampler(adj_list);

		for (size_t i = 0; i < repetitions; i++) {
			vertex s = sampler.get_vertex();
			vertex t = sampler.get_vertex();
			bool found_cut = preflow::pair_vertex_connectivity(adj_list, s, t, cut, k); // TODO return true and stop early if we actually find something
			if (found_cut)
				return true;
		}
		return false;
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_vertex_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();

		std::unordered_set<vertex> cut;
		bool found_cut;

		for (size_t s = 2; s <= a; s *= 2) {    // 2 <= s <= a
			size_t volume = s * (s + k);
			for (size_t i = 0; i < n; i += s) { // ceil(n/s) times per s
				vertex x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					std::unordered_set<vertex> unsplit_L;
					for (vertex v_split : cut) {
						if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
							unsplit_L.insert(v_split / 2);
					}
					std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
					return { unsplit_S, false }; // Found a cut.
				}
				g_split_rev.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					std::unordered_set<vertex> unsplit_L;
					for (vertex v_split : cut) {
						if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
							unsplit_L.insert(v_split / 2);
					}
					std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
					return { unsplit_S, false }; // Found a cut.
				}
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_mixed_vertex_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		size_t i = 0;
		for (; i <= 2 * n; ++i) {
			size_t s = 2 * ((i + 1) & (~i)); // The unique bit that turns 0 -> 1 if we increment i.
			if (s > a)
				continue;
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = s * (s + k);
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		for (size_t i = 0; i <= 2 * n; ++i) {
			size_t s = 2 * ((i + 1) & (~i)); // The unique bit that turns 0 -> 1 if we increment i.
			if (s > a)
				continue;
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = (2 * s - 1) * k;
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_mixed_vertex_sparsified_sampling_skiplow(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		size_t min = 4;
		size_t max = 2 * n / min;
		for (size_t i = 0; i <= max; ++i) {
			size_t s = min * ((i + 1) & (~i)); // The unique bit that turns 0 -> 1 if we increment i.
			if (s > a)
				continue;
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = (2 * s - 1) * k;
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_hybrid_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		for (size_t s = 2; s <= a; s *= 2) {    // 2 <= s <= a
			size_t volume = 2 * s;
			size_t i = 0;
			for (; i < m; i += s) { // ceil(m/s) times per s
				vertex x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " sample # = " << i/s << " x = " << x << std::endl;
#endif
					std::unordered_set<vertex> unsplit_L;
					for (vertex v_split : cut) {
						if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
							unsplit_L.insert(v_split / 2);
					}
					std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
					return { unsplit_S, false }; // Found a cut.
				}
				g_split_rev.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " sample # = " << i / s << " x = " << x << std::endl;
#endif
					std::unordered_set<vertex> unsplit_L;
					for (vertex v_split : cut) {
						if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
							unsplit_L.insert(v_split / 2);
					}
					std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
					return { unsplit_S, false }; // Found a cut.
				}
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_mixed_hybrid_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		size_t i = 0;
		for (; i <= 2 * m; ++i) {
			size_t s = 2 * ((i + 1) & (~i)); // The unique bit that turns 0 -> 1 if we increment i.
			if (s > a)
				continue;
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = s;
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_mixed2_hybrid_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		size_t i = 0;
		size_t s = 1; // The unique bit that turns 0 -> 1 if we increment i.
		for (; s <= a; ++i, s = 2 * ((i + 1) & (~i))) {
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = 16 * s; // Multiply by at least 2.
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_small_hybrid_sampling(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		for(size_t s = 2; s <= a; s *= 2)
		for(size_t i = 0; i < a; i += s) {
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = 2 * s;
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_mixed_hybrid_sampling_skiplow(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		size_t min = 4;
		size_t max = 2 * m / min;
		for (size_t i = 0; i <= max; ++i) {
			size_t s = min * ((i + 1) & (~i)); // The unique bit that turns 0 -> 1 if we increment i.
			if (s > a)
				continue;
			vertex x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = s;
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
			g_split_rev.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
				std::unordered_set<vertex> unsplit_L;
				for (vertex v_split : cut) {
					if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
						unsplit_L.insert(v_split / 2);
				}
				std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
				return { unsplit_S, false }; // Found a cut.
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}

#ifdef WIN32
	std::pair<std::unordered_set<vertex>, bool> vertex_connectivity_unbalanced_hybrid_lp(globalvc_graphs& graphs, size_t k, size_t a, localec_fn localec) {
		graph& g = graphs.g;
		graph& g_split = graphs.g_split;
		graph& g_rev = graphs.g_rev;
		graph& g_split_rev = graphs.g_rev_split;
		size_t n = g.size_vertices();
		size_t m = g.size_edges();

		std::unordered_set<vertex> cut;
		bool found_cut;

		double target_probability = 0.5; // allowed probability of false negatives.
		std::vector<size_t> samplesize;
		std::vector<size_t> volumes;
		std::tie(samplesize, volumes) = optim::optimise_sample_sizes_lp(target_probability, m, k, a);

		//std::cout << "Samples:" << std::endl;
		//for (size_t i = 0; i < volumes.size(); i++) {
		//	std::cout << volumes[i] << ": " << samplesize[i] << std::endl;
		//}

		for (size_t i = 0; i < volumes.size(); i++) {
			size_t volume = volumes[i];
			for (size_t j = 0; j < samplesize[i]; j++) {
				vertex x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at volume = " << volume << " sample # = " << j << " x = " << x << std::endl;
#endif
					std::unordered_set<vertex> unsplit_L;
					for (vertex v_split : cut) {
						if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
							unsplit_L.insert(v_split / 2);
					}
					std::unordered_set<vertex> unsplit_S = get_neighbors(g, unsplit_L);
					return { unsplit_S, false }; // Found a cut.
				}
				g_split_rev.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at volume = " << volume << " sample # = " << j << " x = " << x << std::endl;
#endif
					std::unordered_set<vertex> unsplit_L;
					for (vertex v_split : cut) {
						if (v_split % 2 == 1) // Add vertices whose outvertex is in the split graph cut.
							unsplit_L.insert(v_split / 2);
					}
					std::unordered_set<vertex> unsplit_S = get_neighbors(g_rev, unsplit_L);
					return { unsplit_S, false }; // Found a cut.
				}
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return { {}, true }; // Did not break from the loop with a cut.
	}
#endif

	bool vertex_connectivity_unbalanced_undirected_mixed2_hybrid_sampling(const adj_list& g, graph& g_split, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		size_t n = g_split.size_vertices() / 2;
		size_t m = g_split.size_edges() - n;

		std::unordered_set<vertex> cut;
		bool found_cut;

		/*
		 * Each s is a power of two.
		 * There are roughly 2|E| iterations in total.
		 * The power of two s that is the tightest lower bound for |E| is iterated over once.
		 * Each s occurs proportionally to the inverse of s. (4s is twice as rare as 2s, which is twice as rare as s)
		 * Each s is iterated over |E| / s times (truncated)
		 * 		(TODO confirm)
		 * s higher than 'a' are skipped
		 */
		size_t i = 0;
		size_t s = 1; // The unique bit that turns 0 -> 1 if we increment i.
		const size_t d_vol = max_vol - min_vol + 1;
		for (; s <= d_vol; ++i, s = min_vol * ((i + 1) & (~i))) {
			x = random_int(0, n - 1);
			vertex x_out = 2 * x + 1;
			size_t volume = 2 * s * min_vol; // Multiply by at least 2.
			g_split.reset();
			found_cut = localec(g_split, x_out, volume, k, cut);
			if (found_cut) {
				g_split.reset();
				std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
				std::unordered_set<vertex> S;
				for (vertex v_split : S_split) {
					S.insert(v_split / 2);
				}
#ifdef PRINTFOUNDCUT
				std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
				#endif
				min_cut = S; // Found a cut.
				return true;
			}
		}
#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return false; // Did not break from the loop with a cut.
	}

	bool vertex_connectivity_unbalanced_undirected_vertex_sampling(const adj_list& g, graph& g_split, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		size_t n = g_split.size_vertices() / 2;
		size_t m = g_split.size_edges() - n;

		std::unordered_set<vertex> cut;
		bool found_cut;

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			for (size_t i = 0; i < m; i += s) {
				x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				size_t volume = 2 * s * k; // Multiply by at least 2. k multiplier is for vertex sampling *with* sparsification.
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
					g_split.reset();
					std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
					std::unordered_set<vertex> S;
					for (vertex v_split : S_split) {
						S.insert(v_split / 2);
					}
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					min_cut = S; // Found a cut.
					return true;
				}
			}
		}

#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return false; // Did not break from the loop with a cut.
	}

	bool vertex_connectivity_unbalanced_undirected_edge_sampling(const adj_list& g, graph& g_split, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		size_t n = g_split.size_vertices() / 2;
		size_t m = g_split.size_edges() - n;

		std::unordered_set<vertex> cut;
		bool found_cut;
		auto sampler = edge_source_sampler(g);

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			for (size_t i = 0; i < m; i += s) {
				x = sampler.get_vertex();
				vertex x_out = 2 * x + 1;
				size_t volume = 2 * s; // Multiply by at least 2.
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
					g_split.reset();
					std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
					std::unordered_set<vertex> S;
					for (vertex v_split : S_split) {
						S.insert(v_split / 2);
					}
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					min_cut = S; // Found a cut.
					return true;
				}
			}
		}

#ifdef PRINTFOUNDCUT
		std::cout << "Failed to find a cut." << std::endl;
#endif
		return false; // Did not break from the loop with a cut.
	}

	bool nonstop_vertex_connectivity_unbalanced_undirected_hybrid_sampling(const adj_list &g, graph& g_split, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		size_t n = g_split.size_vertices() / 2;
		size_t m = g_split.size_edges() - n;

		bool found_cut = false;

		for (size_t s = min_vol; s <= max_vol; s *= 2) {
			size_t volume = 2 * s; // Multiply by at least 2.
			for (size_t i = 0; i < m; i += s) {
				x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				g_split.reset();
				std::unordered_set<vertex> cut;
				bool found = localec(g_split, x_out, volume, k, cut);
				if (found) {
					found_cut = true;

					g_split.reset();
					std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
					std::unordered_set<vertex> S;
					for (vertex v_split : S_split) {
						S.insert(v_split / 2);
					}
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					min_cut = S;
					k = S.size();
				}
			}
		}

		return found_cut;
	}

	bool VC_undirected_edge_sampling(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, double a_coeff, std::unordered_set<vertex>& cut) {
		graph g(adj_list);
		graph g_split = make_split_graph(adj_list);
		vertex x;

		size_t n = g.size_vertices();
		size_t m = g.size_edges();
		size_t a = m / (a_coeff * k);

		size_t balanced_repetitions = a_coeff * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if(!found_cut)
				found_cut = unbalancedvc(adj_list, g_split, k, 1, a, localec, x, cut);

		return found_cut;
	}

	bool VC_undirected_vertex_sampling(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, double a_coeff, std::unordered_set<vertex>& cut) {
		graph g_split = make_split_graph(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = g_split.size_edges() - n; // g_split has an extra edge for each vertex.
		//size_t a = (size_t)pow((double)n, a_coeff);
		size_t a = n / (a_coeff * k);

		size_t balanced_repetitions = a_coeff * k; // n/a rounded up
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if (!found_cut)
			found_cut = unbalancedvc(adj_list, g_split, k, 1, a, localec, x, cut);

		return found_cut;
	}


	/*
	 * unbalancedvc: not used
	 * balancedvc: not used
	 * a_coeff: not used
	 */
	bool VC_undirected_edge_sampling_a_by_timing(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, double a_coeff, std::unordered_set<vertex>& cut) {
		const bool debug = false;
		const bool debug2 = false;
		const bool debug3 = false;

		graph g_split = make_split_graph(adj_list);
		edge_source_sampler sampler(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = g_split.size_edges() - n; // g_split has an extra edge for each vertex.

		size_t min_vol = 1;

		std::unordered_set<vertex> edge_cut;
		bool found_cut = false;

		// Step 1: Find the time it takes to run LocalEC (m/s) times at volume s (where s = min_vol) 

		auto t0 = std::chrono::high_resolution_clock::now();
		{
			for (size_t i = 0; i < m; i += min_vol) {
				vertex x = sampler.get_vertex();
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, min_vol, k, edge_cut);
				if (found_cut) {
					std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
					std::unordered_set<vertex> L;
					for (vertex v_split : L_split) {
						if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
							L.insert(v_split / 2);
					}
					std::unordered_set<vertex> S = get_neighbors(adj_list, L);
					cut = S;
					if constexpr (debug2) {
						std::cout << "Found cut at volume = " << min_vol << " i = " << i << " x = " << x << std::endl;
					}
					g_split.reset();
					return true;
				}
			}
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		auto dt = t1 - t0;

		if constexpr (debug) {
			std::cout << "-dt: " << dt.count() << " nanoseconds. " << std::endl;
		}

		// Step 2: Run Ford-Fulkerson until we have spent as much time as we did above.
		// ... BUT at least m/k times because 'a' cannot be more than m/k.

		size_t ff_repetitions = 0;
		const size_t min_ff_repetitions = k;
		t0 = std::chrono::high_resolution_clock::now();

		// At least a minimum number of times.
		for (; ff_repetitions < min_ff_repetitions; ff_repetitions++) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			maxflow::adjacency_list adj_split = make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(adj_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(adj_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(adj_list, L);
				cut = S;
				return true;
			}
		}
		t1 = std::chrono::high_resolution_clock::now();

		// Until enough time has passed.
		for (; t1 - t0 < dt; ff_repetitions++, t1 = std::chrono::high_resolution_clock::now()) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			maxflow::adjacency_list adj_split = make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(adj_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = get_neighbors(adj_list, L);
				cut = S;
				return true;
			}
		}

		size_t a = m / ff_repetitions; // The sample size for ford-fulkerson should be m/a.

		if constexpr (debug) {
			std::cout << "-ff_repetitions: " << ff_repetitions << std::endl;
			std::cout << "-a: " << a << std::endl;
		}
		if constexpr (debug || debug3) {
			std::cout << "-a: " << a << " m: " << m << " k: " << k << " m/a: " << (double)m / (double)a << std::endl;
		}

		for (size_t volume = 2 * min_vol; volume < 2 * a; volume *= 2) {
			for (size_t i = 0; i < m; i += volume) {
				vertex x = sampler.get_vertex();
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, edge_cut);
				if (found_cut) {
					g_split.reset();
					std::unordered_set<vertex> split_cut = get_neighbors(g_split, edge_cut);
					cut.clear();
					for (vertex w : split_cut) {
						cut.insert(w / 2);
					}
					return true;
				}
			}
		}

		return false; // If we had found a cut we would have returned by now.
	}

	bool VC_undirected_edge_sampling_trivial_separately(localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, double a_coeff, std::unordered_set<vertex>& cut) {
		graph g(adj_list);
		graph g_split = make_split_graph(adj_list);
		vertex x;

		size_t n = g.size_vertices();
		size_t m = g.size_edges();
		size_t a = m / (a_coeff * k);

		size_t d_min = SIZE_MAX;
		for (vertex v = 0; v < adj_list.size(); v++) {
			if (d_min > adj_list[v].size()) {
				d_min = adj_list[v].size();
				cut.clear();
				cut.insert(adj_list[v].begin(), adj_list[v].end());
			}
		}

		size_t balanced_repetitions = a_coeff; // m/a rounded up
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if (!found_cut)
			found_cut = unbalancedvc(adj_list, g_split, k, d_min, a, localec, x, cut);

		return found_cut;
	}
} // namespace localvc
