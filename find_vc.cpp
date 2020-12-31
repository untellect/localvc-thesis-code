#include "pch.h"

#include "Localvc.h"
#include "Util.h"
#include "Sparse.h"

#include "PairPreflowPush.h"

//#define VERBOSE_DEBUG
//#define PRINT_G_SIZES
//#define PRINT_VOLUME

/*
 * Helper functions (unbalanced)
 */

/*
size_t starting_point_by_doubling(sparse::labelling& labelling, localvc::globalvc_ud_fn globalvc, localvc::localec_fn localec, versioned_graph::vertex& min_cut_x, std::unordered_set<versioned_graph::vertex>& min_cut, double a_coeff) {
	versioned_graph::vertex x = 0;
	std::unordered_set<versioned_graph::vertex> cut;

	size_t k = 2;
	while (true) {
		auto g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
		std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
		auto g_split = make_split_graph(g);
		size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
		bool found_cut = globalvc(g, g_split, k, a, localec, x, cut);
		if (found_cut) {
			min_cut = cut;
			min_cut_x = x;
#ifdef VERBOSE_DEBUG
			std::cout << min_cut.size() << " <= " << k << " <= k" << std::endl;
#endif
			return k;
		}
#ifdef VERBOSE_DEBUG
		std::cout << "k < " << k << std::endl;
#endif
		k *= 2;
	}
}


size_t binarysearch_helper(size_t& k_low, size_t& k_high, sparse::labelling& labelling, localvc::globalvc_ud_fn globalvc, localvc::localec_fn localec, versioned_graph::vertex& min_cut_x, std::unordered_set<versioned_graph::vertex>& min_cut, double a_coeff) {
	versioned_graph::vertex x;
	std::unordered_set<versioned_graph::vertex> cut;

	while (k_low + 1 < k_high) {
		size_t k = (k_low + k_high) / 2;	// Converge by binary search.

		//size_t k = k_high - 1;			// Converge by scanning from above.
		auto g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
		std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
		auto g_split = make_split_graph(g);
		size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
		bool found_cut = globalvc(g, g_split, k, a, localec, x, cut);
		if (!found_cut) {
			k_low = k; // No cut found, at least k-connected.
#ifdef VERBOSE_DEBUG
			std::cout << k_low << " <= k" << std::endl;
#endif
		}
		else {
			min_cut = cut;
			min_cut_x = x;
			k_high = min_cut.size(); // k; // Found a cut, at most (k-1)-connected.
#ifdef VERBOSE_DEBUG
			std::cout << "k < " << k_high << std::endl;
#endif
		}
	}
#ifdef VERBOSE_DEBUG
	std::cout << "k might be " << k_low << std::endl;
#endif
	return k_low;
}

void boosting(size_t& k, sparse::labelling& labelling, localvc::globalvc_ud_fn globalvc, localvc::localec_fn localec, versioned_graph::vertex& min_cut_x, std::unordered_set<versioned_graph::vertex>& min_cut, double a_coeff, size_t boost) {
	versioned_graph::vertex x;
	std::unordered_set<versioned_graph::vertex> cut;
	auto g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
	std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
	auto g_split = make_split_graph(g);
	size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
	for (size_t i = 0; i < boost; i++) {
		bool found_cut = globalvc(g, g_split, k, a, localec, x, cut);

		if (found_cut) { // Found a cut.
#ifdef VERBOSE_DEBUG
			std::cout << "k < " << k << std::endl;
#endif
			// Assume the new cut is the min cut.
			min_cut_x = x;
			min_cut = cut;
			k = min_cut.size(); // k -= 1;
			// Rebuild the graphs.
			g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
			std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
			g_split = make_split_graph(g);
			a = (size_t)std::pow((double)g.size_edges(), a_coeff);
			// Restart the boosting loop.
			i = 0;
			continue;
		}

		if (!found_cut) continue; // Did not find a cut.


	}
}
*/

/*
 * Actual find_vc versions
 */

namespace localvc {
	/*
	size_t find_vertex_connectivity_undirected_binarysearch(const std::vector<std::vector<vertex>>& adj_list, globalvc_ud_fn globalvc, localec_fn localec, double a_coeff, vertex& min_cut_x, std::unordered_set<vertex>& min_cut, size_t boost) {
		std::pair<std::unordered_set<vertex>, bool> cutpair = { {}, true };
		vertex x = 0;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		size_t k_low = starting_point_by_doubling(labelling, globalvc, localec, min_cut_x, min_cut, a_coeff) / 2;
		size_t k_high = min_cut.size(); // k_low * 2;

		size_t k = binarysearch_helper(k_low, k_high, labelling, globalvc, localec, min_cut_x, min_cut, a_coeff);

		boosting(k, labelling, globalvc, localec, min_cut_x, min_cut, a_coeff, boost);

#ifdef PRINT_VOLUME
		graph g = graph(adj_list);
		size_t v = get_vertex_cut_volume(g, min_cut_x, min_cut);
		std::cout << "Volume for one minimum cut: " << v << std::endl;
#endif

		return k;
	}


	size_t find_vertex_connectivity_undirected_no_binarysearch(const std::vector<std::vector<vertex>>& adj_list, globalvc_ud_fn globalvc, localec_fn localec, double a_coeff, vertex& min_cut_x, std::unordered_set<vertex>& min_cut, size_t boost) {
		std::pair<std::unordered_set<vertex>, bool> cutpair = { {}, true };
		vertex x = 0;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		size_t k_low = starting_point_by_doubling(labelling, globalvc, localec, min_cut_x, min_cut, a_coeff) / 2;
		size_t k_high = min_cut.size(); // k_low * 2;

		size_t k = k_high;
		//size_t k = binarysearch_helper(k_low, k_high, labelling, globalvc, localec, min_cut_x, min_cut, a_coeff);

		boosting(k, labelling, globalvc, localec, min_cut_x, min_cut, a_coeff, boost);

#ifdef PRINT_VOLUME
		graph g = graph(adj_list);
		size_t v = get_vertex_cut_volume(g, min_cut_x, min_cut);
		std::cout << "Volume for one minimum cut: " << v << std::endl;
#endif

		return k;
	}
*/

	void find_vertex_connectivity_undirected_hybrid_sampling(const std::vector<std::vector<vertex>>& adj_list, localec_fn localec, double a_coeff, vertex& min_cut_x, std::unordered_set<vertex>& min_cut, size_t boost) {
		std::unordered_set<vertex> cut;
		bool found_cut;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		// Start with very high k.
		size_t k = adj_list.size();
		// Bound k by minimum degree.
		for (vertex x = 0; x < adj_list.size(); x++) {
			if (adj_list[x].size() < k) {
				min_cut_x = x;
				min_cut = std::unordered_set<vertex>(adj_list[x].begin(), adj_list[x].end());
				k = min_cut.size();
			}
		}

		graph g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
		std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
		graph g_split = make_split_graph(g);
		size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
		size_t n = g.size_vertices();
		size_t m = g.size_edges();



		for (size_t s = 2; s <= a; s *= 2) {
			size_t volume = 16 * s; // Multiply by at least 2.
			for (size_t i = 0; i < a * boost; i += s) { // "i < m*boost" if we go by the articles.
				vertex x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
					g_split.reset();
					std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
					std::unordered_set<vertex> S;
					for (vertex v_split : S_split) {
						S.insert(v_split / 2);
					}
					if (S.size() < min_cut.size()) {
						// Found a new min cut.
						min_cut = S;
						min_cut_x = x;

						// Reduce k for subsequent iterations to only look for smaller cuts.
						k = min_cut.size();
						graph g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
						std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
						graph g_split = make_split_graph(g);
						size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
						size_t n = g.size_vertices();
						size_t m = g.size_edges();
					}
				}
			}
		}
	}

	void find_vertex_connectivity_undirected_mixed2_hybrid_sampling(const std::vector<std::vector<vertex>>& adj_list, localec_fn localec, double a_coeff, vertex& min_cut_x, std::unordered_set<vertex>& min_cut, size_t boost) {
		std::unordered_set<vertex> cut;
		bool found_cut;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		// Start with very high k.
		size_t k = adj_list.size();
		// Bound k by minimum degree.
		for (vertex x = 0; x < adj_list.size(); x++) {
			if (adj_list[x].size() < k) {
				min_cut_x = x;
				min_cut = std::unordered_set<vertex>(adj_list[x].begin(), adj_list[x].end());
				k = min_cut.size();
			}
		}

		graph g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
		std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
		graph g_split = make_split_graph(g);
		size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
		size_t n = g.size_vertices();
		size_t m = g.size_edges();


		for (size_t j = 0; j < boost; j++) {
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
					g_split.reset();
					std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
					std::unordered_set<vertex> S;
					for (vertex v_split : S_split) {
						S.insert(v_split / 2);
					}
					if (S.size() < min_cut.size()) {
						// Found a new min cut.
						min_cut = S;
						min_cut_x = x;

						// Reduce k for subsequent iterations to only look for smaller cuts.
						k = min_cut.size();
						graph g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
						std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
						graph g_split = make_split_graph(g);
						size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
						size_t n = g.size_vertices();
						size_t m = g.size_edges();
					}
				}
			}
		}



		for (size_t s = 2; s <= a; s *= 2) {
			size_t volume = 16 * s; // Multiply by at least 2.
			for (size_t i = 0; i < a * boost; i += s) { // "i < m*boost" if we go by the articles.
				vertex x = random_int(0, n - 1);
				vertex x_out = 2 * x + 1;
				g_split.reset();
				found_cut = localec(g_split, x_out, volume, k, cut);
				if (found_cut) {
					g_split.reset();
					std::unordered_set<vertex> S_split = get_neighbors(g_split, cut);
					std::unordered_set<vertex> S;
					for (vertex v_split : S_split) {
						S.insert(v_split / 2);
					}
					if (S.size() < min_cut.size()) {
						// Found a new min cut.
						min_cut = S;
						min_cut_x = x;

						// Reduce k for subsequent iterations to only look for smaller cuts.
						k = min_cut.size();
						graph g = sparse::nagamochi_ibraki_graph(labelling, k);
#ifdef PRINT_G_SIZES
						std::cout << "G_" << k << " has " << g.size_edges() << " edges." << std::endl;
#endif
						graph g_split = make_split_graph(g);
						size_t a = (size_t)std::pow(g.size_edges(), a_coeff);
						size_t n = g.size_vertices();
						size_t m = g.size_edges();
					}
				}
			}
		}
	}

	/*
	size_t find_vertex_connectivity_undirected_balanced(const std::vector<std::vector<vertex>>& adj_list, globalvc_balanced_fn globalvc, size_t repetitions, size_t boost = 100) {
		size_t k = 1;
		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		std::unordered_set<vertex> min_cut;

		while (true) {
			graph g = sparse::nagamochi_ibraki_graph(labelling, k);
			graph g_split = make_split_graph(g);
			auto cutpair = globalvc(g, g_split, k, repetitions);
			if (cutpair.second == false) {
				min_cut = cutpair.first;
				k = min_cut.size();
				break;
			}
			k *= 2;
		}

		for (size_t i = 0; i < boost; i++) {
			graph g = sparse::nagamochi_ibraki_graph(labelling, k);
			graph g_split = make_split_graph(g);
			auto cutpair = globalvc(g, g_split, k, repetitions);
			if (cutpair.second == false) { // Found a cut. 
				min_cut = cutpair.first;
				k = min_cut.size();
				// i = 0; // Actually, why bother restarting the loop?
			}
		}

		return k;
	}
	*/

	std::unordered_set<vertex> find_vertex_connectivity_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adj_list, double a_coeff, size_t boost) {
		const bool debug = false;
		
		std::unordered_set<vertex> cut;

		auto labelling = sparse::nagamochi_ibraki_labelling(adj_list);

		bool found_cut = false;
		for (size_t k = 1; !found_cut; k *= 2) {
			auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
			found_cut = vc(localec, unbalancedvc, balancedvc, sparsified, k, a_coeff, cut);
			if constexpr (debug) {
				if (!found_cut)
					std::cout << "No cut at k=" << k << std::endl;
				else
					std::cout << "Cut at k=" << k << std::endl;
			}
		}
		
		size_t k = cut.size();
		if (k == 0) return cut;
		auto sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k);
		for (size_t i = 0; i < boost; ) {
			i++;
			if (vc(localec, unbalancedvc, balancedvc, sparsified, k, a_coeff, cut)) {
				k = cut.size();
				sparsified = sparse::nagamochi_ibraki_adj_list(labelling, k); // Re-sparsify if we find a better cut.
				i = 0; // We don't restart boosting completely but we don't count iterations where a cut is found.
				if constexpr (debug) {
					std::cout << i << " Cut at k=" << k << std::endl;
				}
			}
			else if constexpr (debug) {
				std::cout << i << " No cut at k=" << k << std::endl;
			}
		}

		return cut;
	}


}
