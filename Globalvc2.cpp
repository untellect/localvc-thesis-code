#include "pch.h"

#include "localvc2.h"
#include "MaxFlow.h"
#include "Sparse.h"
#include "Util.h"
#include <chrono>

namespace localvc2 {
	bool vertex_connectivity_unbalanced_skip(const adjlist& g, graph& sg, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		return false;
	}

	bool vertex_connectivity_unbalanced_undirected_edge_sampling(const adjlist& g, graph& sg, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		const bool debug = false;

		size_t n = g.size();
		size_t m = 0;
		for (auto& adj : g)
			m += adj.size();

		auto sampler = versioned_graph::edge_source_sampler(g); // borrowing some code here.
		std::unordered_set<vertex> cut;

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			for (size_t i = 0; i < m; i += s) {
				vertex v = sampler.get_vertex();
				vertex v_out = 2 * v + 1;
				size_t volume = 2 * s;
				sg.reset();
				bool found_cut = localec(sg, v_out, volume, k, cut);

				if constexpr (debug) {
					if (found_cut)
						std::cout << "\t\tk': " << cut.size() << std::endl;
				}

				if (found_cut) {
					sg.reset();
					std::unordered_set<vertex> split_cut = get_neighbors(sg, cut);
					if constexpr (debug) {
						std::cout << "\t\tk'': " << split_cut.size() << std::endl;
					}
					min_cut.clear();
					for (vertex w : split_cut) {
						min_cut.insert(w / 2);
					}
					if constexpr (debug) {
						std::cout << "\t\tk''': " << min_cut.size() << std::endl;
					}
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					x = v;
					return true;
				}
			}
		}

		return false;
	}

	bool vertex_connectivity_unbalanced_undirected_finish_edge_sampling(const adjlist& g, graph& sg, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, std::unordered_set<vertex>& min_cut) {
		const bool debug = false;

		size_t n = g.size();
		size_t m = 0;
		for (auto& adj : g)
			m += adj.size();

		auto sampler = versioned_graph::edge_source_sampler(g); // borrowing some code here.
		std::unordered_set<vertex> cut;

		bool found_cut = false;

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			for (size_t i = 0; i < m; i += s) {
				vertex v = sampler.get_vertex();
				vertex v_out = 2 * v + 1;
				size_t volume = 2 * s;
				sg.reset();
				bool found_new_cut = localec(sg, v_out, volume, k, cut);

				if constexpr (debug) {
					if (found_new_cut)
						std::cout << "\t\tk': " << cut.size() << std::endl;
				}

				if (found_new_cut) {
					found_cut = found_new_cut;

					sg.reset();
					std::unordered_set<vertex> split_cut = get_neighbors(sg, cut);
					if constexpr (debug) {
						std::cout << "\t\tk'': " << split_cut.size() << std::endl;
					}
					min_cut.clear();
					for (vertex w : split_cut) {
						min_cut.insert(w / 2);
					}
					if constexpr (debug) {
						std::cout << "\t\tk''': " << min_cut.size() << std::endl;
					}
#ifdef PRINTFOUNDCUT
					std::cout << "Found cut at s = " << s << " volume = " << volume << " i = " << i << " x = " << x << std::endl;
#endif
					x = v;
					k = min_cut.size();
				}
			}
		}

		return found_cut;
	}

	bool VC_undirected_edge_sampling(localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut) {
		const bool debug = false;

		graph sg = make_split_graph(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = 0;
		for (auto& adj : adj_list)
			m += adj.size();
		size_t a = m / (3 * k);

		if (m == 0) {
			if constexpr (debug) {
				std::cout << "\tEmpty graph." << std::endl;
			}

			cut.clear();
			return true;
		}

		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if (!found_cut) {
			found_cut = unbalancedvc(adj_list, sg, k, 1, a, localec, x, cut);
		}


		if constexpr (debug) {
			std::cout << "\tCut size: " << cut.size() << std::endl;
		}
		return found_cut;
	}

	bool VC_undirected_finish_edge_sampling(localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut) {
		const bool debug = false;

		graph sg = make_split_graph(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = 0;
		for (auto& adj : adj_list)
			m += adj.size();
		size_t a = m / (3 * k);

		if (m == 0) {
			if constexpr (debug) {
				std::cout << "\tEmpty graph." << std::endl;
			}

			cut.clear();
			return true;
		}

		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if (found_cut) k = cut.size();
		found_cut = unbalancedvc(adj_list, sg, k, 1, a, localec, x, cut) || found_cut;

		if constexpr (debug) {
			std::cout << "\tCut size: " << cut.size() << std::endl;
		}
		return found_cut;
	}

	// Check trivial cuts first separately. This means we can skip the lowest volume LocalEC iterations while achieving 100% accuracy for those volumes.
	bool VC_undirected_edge_sampling_trivial_separately(localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut) {
		const bool debug = false;

		graph sg = make_split_graph(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = 0;
		size_t d_min = SIZE_MAX;
		for (vertex v = 0; v < adj_list.size(); v++) {
			m += adj_list[v].size();
			if (d_min > adj_list[v].size()) {
				d_min = adj_list[v].size();
				if (d_min < k) {
					cut.clear();
					cut.insert(adj_list[v].begin(), adj_list[v].end());
				}
			}
		}

		size_t a = m / (3 * k);

		if (m == 0) {
			if constexpr (debug) {
				std::cout << "\tEmpty graph." << std::endl;
			}

			cut.clear();
			return true;
		}

		// m/a = m/(m/2k) = 2k *but* each Ford-Fulkerson call samples *two* equivalent endpoints in an undirected graph.
		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divided by two because we sample two points every time.
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if (!found_cut) {
			found_cut = unbalancedvc(adj_list, sg, k, d_min, a, localec, x, cut);
		}


		if constexpr (debug) {
			std::cout << "\tCut size: " << cut.size() << std::endl;
		}
		return found_cut;
	}
	
	bool VC_undirected_finish_edge_sampling_trivial_separately(localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut) {
		const bool debug = false;

		graph sg = make_split_graph(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = 0;
		size_t d_min = SIZE_MAX;

		for (vertex v = 0; v < adj_list.size(); v++) {
			m += adj_list[v].size();
			if (d_min > adj_list[v].size()) {
				d_min = adj_list[v].size();
				if (d_min < k) {
					cut.clear();
					cut.insert(adj_list[v].begin(), adj_list[v].end());
				}
			}
		}

		if (m == 0) {
			if constexpr (debug) {
				std::cout << "\tEmpty graph." << std::endl;
			}

			cut.clear();
			return true;
		}

		size_t a = m / (3 * k);

		// m/a = m/(m/2k) = 2k *but* each Ford-Fulkerson call samples *two* equivalent endpoints in an undirected graph.
		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divided by two because we sample two points every time.
		bool found_cut = balancedvc(adj_list, k, balanced_repetitions, cut);
		if (found_cut) k = cut.size();
		found_cut = unbalancedvc(adj_list, sg, k, 1, a, localec, x, cut) || found_cut;

		if constexpr (debug) {
			std::cout << "\tCut size: " << cut.size() << std::endl;
		}
		return found_cut;
	}



	// unbalancedvc, balancedvc and a_coeff are *unused*
	bool VC_undirected_edge_sampling_a_by_timing(localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, size_t k, std::unordered_set<vertex>& cut) {
		const bool debug = false;
		const bool debug2 = false;
		const bool debug3 = false;

		graph g_split = make_split_graph(adj_list);
		versioned_graph::edge_source_sampler sampler(adj_list);
		vertex x;

		size_t n = adj_list.size();
		size_t m = 0;
		for (auto& adj : adj_list)
			m += adj.size();

		size_t min_vol = 2;

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
					g_split.reset();
					std::unordered_set<vertex> split_cut = get_neighbors(g_split, edge_cut);
					cut.clear();
					for (vertex w : split_cut) {
						cut.insert(w / 2);
					}
					if constexpr (debug2) {
						std::cout << "[c] Found cut at volume = " << min_vol << " i = " << i << " x = " << x << std::endl;
					}
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
			maxflow::adjacency_list adj_split = localvc::make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(adj_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = localvc::get_reachable(adj_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = localvc::get_neighbors(adj_list, L);
				cut = S;
				if constexpr (debug2) {
					std::cout << "L_split.size() = " << L_split.size() << std::endl;
					std::cout << "L.size() = " << L.size() << std::endl;
					std::cout << "S.size() = " << S.size() << std::endl;
					std::cout << "[d] Found cut at ff_repetitions = " << ff_repetitions << std::endl;
				}
				return true;
			}
		}
		t1 = std::chrono::high_resolution_clock::now();

		// Until enough time has passed.
		for (; t1 - t0 < dt; ff_repetitions++, t1 = std::chrono::high_resolution_clock::now()) {
			vertex x_out = 2 * sampler.get_vertex() + 1;
			vertex y_in = 2 * sampler.get_vertex();
			maxflow::adjacency_list adj_split = versioned_graph::make_split_graph_adj_list(adj_list);

			size_t k_xy = maxflow::ford_fulkerson_adj(adj_split, x_out, y_in, k);
			if (k_xy < k) {
				std::unordered_set<vertex> L_split = get_reachable(g_split, x_out);
				std::unordered_set<vertex> L;
				for (vertex v_split : L_split) {
					if (v_split % 2 == 1) // Do not add vertices where the edge (v_in,v_out) is cut.
						L.insert(v_split / 2);
				}
				std::unordered_set<vertex> S = localvc::get_neighbors(adj_list, L);
				cut = S;
				if constexpr (debug2) {
					std::cout << k_xy << " = k_xy < k = " << k << std::endl;
					std::cout << "L_split.size() = " << L_split.size() << std::endl;
					std::cout << "L.size() = " << L.size() << std::endl;
					std::cout << "S.size() = " << S.size() << std::endl;
					std::cout << "[e] Found cut at ff_repetitions = " << ff_repetitions << std::endl;
				}
				return true;
			}
		}

		size_t a = m / ff_repetitions; // The sample size for ford-fulkerson should be m/a.

		if constexpr (debug) {
			std::cout << "-ff_repetitions: " << ff_repetitions << std::endl;
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
					if constexpr (debug2) {
						std::cout << "[f] Found cut at volume = " << volume << " i = " << i << " x = " << x << std::endl;
					}
					return true;
				}
			}
		}

		return false; // If we had found a cut we would have returned by now.
	}

	std::unordered_set<vertex> find_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, double a_coeff, size_t boost) {
		const bool debug = false;

		std::unordered_set<vertex> cut;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		bool found_cut = false;
		for (size_t k = 1; !found_cut; k *= 2) {
			auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
			found_cut = vc(localec, unbalancedvc, balancedvc, sparsified, k, cut);
			if constexpr (debug) {
				if (found_cut && cut.size() >= k)
					std::cout << "Found an impossible vertex separator!" << std::endl;
			}

			if constexpr (debug) {
				if (!found_cut)
					std::cout << "[a] No cut at k=" << k << std::endl;
				else
					std::cout << "[a] Cut at k=" << k << std::endl;
			}
		}

		size_t k = cut.size();
		if constexpr (debug) {
			std::cout << "k=" << k << std::endl;
		}
		if (k == 0) return cut;
		auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
		for (size_t i = 0; i < boost; ) {
			i++;
			if (vc(localec, unbalancedvc, balancedvc, sparsified, k, cut)) {
				k = cut.size();
				sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k); // Re-sparsify if we find a better cut.
				i = 0; // We don't restart boosting completely but we don't count iterations where a cut is found.
				if constexpr (debug) {
					std::cout << i << "[b] Cut at k=" << k << std::endl;
				}
				if (k == 0) break;
			}
			else if constexpr (debug) {
				std::cout << i << "[b] No cut at k=" << k << std::endl;
			}
		}

		return cut;
	}

	std::unordered_set<vertex> find_first_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list) {
		const bool debug = false;

		std::unordered_set<vertex> cut;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		bool found_cut = false;
		for (size_t k = 1; !found_cut; k *= 2) {
			auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
			found_cut = vc(localec, unbalancedvc, balancedvc, sparsified, k, cut);

			if constexpr (debug) {
				if (found_cut && cut.size() >= k)
					std::cout << "Found an impossible vertex separator!" << std::endl;
				if (!found_cut)
					std::cout << "[a] No cut at k=" << k << std::endl;
				else
					std::cout << "[a] Cut at k=" << k << std::endl;
			}
		}

		//size_t k = cut.size();
		//if constexpr (debug) {
		//	std::cout << "k=" << k << std::endl;
		//}
		//if (k == 0) return cut;
		//auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
		//for (size_t i = 0; i < 2; i++) {
		//	if (vc(localec, unbalancedvc, balancedvc, sparsified, k, a_coeff, cut)) {
		//		k = cut.size();
		//		sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k); // Re-sparsify if we find a better cut.
		//		i = 0; // We don't restart boosting completely but we don't count iterations where a cut is found.
		//		if constexpr (debug) {
		//			std::cout << i << "[b] Cut at k=" << k << std::endl;
		//		}
		//		if (k == 0) break;
		//	}
		//	else if constexpr (debug) {
		//		std::cout << i << "[b] No cut at k=" << k << std::endl;
		//	}
		//}

		return cut;
	}

	std::unordered_set<vertex> find_and_improve_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, localvc::globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list) {
		const bool debug = false;

		std::unordered_set<vertex> cut;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		bool found_cut = false;
		for (size_t k = 1; !found_cut; k *= 2) {
			auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
			found_cut = vc(localec, unbalancedvc, balancedvc, sparsified, k, cut);

			if constexpr (debug) {
				if (found_cut && cut.size() >= k)
					std::cout << "Found an impossible vertex separator!" << std::endl;
				if (!found_cut)
					std::cout << "[a] No cut at k=" << k << std::endl;
				else
					std::cout << "[a] Cut at k=" << k << std::endl;
			}
		}

		if constexpr (debug) {
			std::cout << "k=" << cut.size() << std::endl;
		}

		while (found_cut && !cut.empty()) {
			size_t k = cut.size();
			auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
			found_cut = vc(localec, unbalancedvc, balancedvc, sparsified, k, cut);
			if constexpr (debug) {
				if (found_cut)
					std::cout << "[b] Cut at k=" << k << std::endl;
				else
					std::cout << "[b] No cut at k=" << k << std::endl;
			}
		}

		return cut;
	}
} // namespace localvc2
