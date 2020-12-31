#include "pch.h"

#include "preprocessing.h"
#include "Util.h"


// Gets the vertices that are included in the k-core. G must be undirected/bidirectional
std::vector<size_t> preprocessing::kcore_vertices_undirected(adj_list& G, size_t k) {
	size_t n = G.size();
	std::vector<bool> removed(n, false);
	std::vector<size_t> to_be_removed;

	std::vector<size_t> d(n);

	for (size_t v = 0; v < n; v++) {
		d[v] = G[v].size();
		if (d[v] < k)
			to_be_removed.push_back(v);
	}

	while (!to_be_removed.empty()) {
		size_t v = to_be_removed.back();
		to_be_removed.pop_back();
		if (removed[v]) continue;

		for (size_t w : G[v]) {
			if (d[w] == k)
				to_be_removed.push_back(w);
			d[w]--;
		}
		removed[v] = true;
	}

	std::vector<size_t> non_removed_vertices;
	for (size_t v = 0; v < n; v++) {
		if (!removed[v])
			non_removed_vertices.push_back(v);
	}

	return non_removed_vertices;
}

// Gets the subgraph G(V) with all the indices shifted.
preprocessing::adj_list preprocessing::get_subgraph(adj_list& G, std::vector<size_t> V) {
	size_t n = G.size(); // old n
	size_t n_v = V.size(); // new n;

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	for (size_t i = 0; i < n_v; i++) {
		old_to_new_mapping[V[i]] = i;
	}

	adj_list G_V(n_v);
	// For each vertex v, copy over the old adjacency list, translating them to the new mapping and skipping any removed vertices.
	for (size_t v = 0; v < n_v; v++) {
		size_t v_old = V[v];
		for (size_t w_old : G[v_old]) {
			size_t w = old_to_new_mapping[w_old];
			if (w == SIZE_MAX) continue;
			G_V[v].push_back(w); // Other direction will be added when we iterate over G[w]
		}
	}

	return G_V;
}


void preprocessing::in_place_subgraph(adj_list& G, std::vector<size_t>& V) {
	size_t n = G.size(); // old n
	size_t n_v = V.size(); // new n;

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	for (size_t i = 0; i < n_v; i++) {
		old_to_new_mapping[V[i]] = i;
	}

	std::sort(V.begin(), V.end());

	// Move adjacency lists around.
	for (size_t v_new = 0; v_new < n_v; v_new++) {
		size_t v_old = V[v_new];

		if (v_old == v_new) {
			for(size_t& w : G[v_new])
				w = old_to_new_mapping[w];
			auto& it = std::remove(G[v_new].begin(), G[v_new].end(), SIZE_MAX);
			G[v_new].erase(it, G[v_new].end());
		}
		else {
			G[v_new].clear();
			for (size_t w_old : G[v_old]) {
				size_t w = old_to_new_mapping[w_old];
				if (w == SIZE_MAX) continue;
				G[v_new].push_back(w);
			}
		}
	}

	G.resize(n_v);
}

// Only removes edges.
void preprocessing::in_place_k_core_undirected(adj_list& G, size_t k) {
	size_t n = G.size();
	std::vector<size_t> to_be_removed;

	for (size_t v = 0; v < n; v++) {
		if (G[v].size() < k)
			to_be_removed.push_back(v);
	}

	while (!to_be_removed.empty()) {
		size_t v = to_be_removed.back();
		to_be_removed.pop_back();
		for (size_t w : G[v]) {
			if (G[w].size() == k)
				to_be_removed.push_back(w);
			std::vector<size_t>::iterator& it = std::find(G[w].begin(), G[w].end(), v);
			std::swap(*it, G[w].back());
			G[w].pop_back();
		}
		G[v].clear();
	}
}

void preprocessing::in_place_LWCC(adj_list& G) {
	auto& V = LCC_vertices_undirected(G);

	/*
	size_t n = G.size();
	size_t n_v = V.size();

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	for (size_t i = 0; i < n_v; i++) {
		old_to_new_mapping[V[i]] = i;
	}
	std::sort(V.begin(), V.end());

	for (size_t v_new = 0; v_new < n_v; v_new++) {
		size_t v_old = V[v_new];

		std::vector<size_t> adj_v;
		for (size_t w_old : G[v_old]) {
			size_t w_new = old_to_new_mapping[w_old];
			if (w_new == SIZE_MAX) continue;
			adj_v.push_back(w_new);
		}

		G[v_new].assign(adj_v.begin(), adj_v.end());
	}

	G.resize(n_v);
	*/

	//G = get_subgraph(G, V);
	in_place_subgraph(G, V);
}

void preprocessing::in_place_LCC_of_kcore(adj_list& G, size_t k) {
	in_place_k_core_undirected(G, k);
	in_place_LWCC(G);
}

// All connected components returned as vectors of vertices.
std::vector<std::vector<size_t>> preprocessing::all_CC_vertices_undirected(adj_list& G) {
	size_t n = G.size();

	std::vector<std::vector<size_t>> all_CC_vertices;

	std::vector<bool> visited(n, false);
	for (size_t starting_vertex = 0; starting_vertex < n; starting_vertex++) {
		if (visited[starting_vertex]) continue;

		std::vector<size_t> CC_vertices;
		std::vector<size_t> Q = { starting_vertex };
		while (!Q.empty()) {
			size_t v = Q.back();
			Q.pop_back();

			if (visited[v]) continue;
			visited[v] = true;
			CC_vertices.push_back(v);

			for (size_t w : G[v])
				Q.push_back(w);
		}

		all_CC_vertices.push_back(CC_vertices);
	}

	return all_CC_vertices;
}

// Largest Connected Component (most vertices), returned as a vector of vertices.
std::vector<size_t> preprocessing::LCC_vertices_undirected(adj_list& G) {
	size_t n = G.size();

	std::vector<size_t> LCC_vertices;

	std::vector<bool> visited(n, false);
	for (size_t starting_vertex = 0; starting_vertex < n; starting_vertex++) {
		if (visited[starting_vertex]) continue;

		std::vector<size_t> CC_vertices;
		std::vector<size_t> Q = { starting_vertex };
		while (!Q.empty()) {
			size_t v = Q.back();
			Q.pop_back();

			if (visited[v]) continue;
			visited[v] = true;
			CC_vertices.push_back(v);

			for (size_t w : G[v])
				Q.push_back(w);
		}

		if (CC_vertices.size() > LCC_vertices.size())
			LCC_vertices = CC_vertices;
	}

	return LCC_vertices;
}

preprocessing::adj_list preprocessing::LCC_of_kcore(adj_list& G, size_t k) {
	adj_list G_temp = get_subgraph(G, kcore_vertices_undirected(G, k));
	return get_subgraph(G_temp, LCC_vertices_undirected(G_temp));
}

std::vector<preprocessing::adj_list> preprocessing::CC_after_cut(adj_list& G, std::unordered_set<size_t> separator) {
	size_t n = G.size();
	std::vector<adj_list> CCs; // connected components

	std::vector<bool> visited(n, false);

	for (size_t starting_vertex = 0; starting_vertex < n; starting_vertex++) {
		if (visited[starting_vertex]) continue;

		std::vector<size_t> CC_vertices;
		std::vector<size_t> Q = { starting_vertex };
		while (!Q.empty()) {
			size_t v = Q.back();
			Q.pop_back();
			if (visited[v])
				continue;
			CC_vertices.push_back(v);
			// Include cut vertices in both connected components (why not?) but do not continue DFS through them.
			if (separator.find(v) != separator.end())
				continue;
			visited[v] = true;


			for (size_t w : G[v])
				Q.push_back(w);
		}
		CCs.push_back(get_subgraph(G, CC_vertices));
	}

	return CCs;
}

std::vector<preprocessing::adj_list> preprocessing::k_connected_components(adj_list& G, size_t k) {
	localvc::VC_fn* vc = localvc::VC_undirected_edge_sampling;
	localvc::localec_fn* localec = localvc::local_edge_connectivity_v2;
	localvc::globalvc_ud_fn* unbalancedvc = localvc::vertex_connectivity_unbalanced_undirected_edge_sampling;
	localvc::globalvc_balanced_fn* balancedvc = localvc::vertex_connectivity_balanced_edge_sampling_adj_list;

	std::vector<adj_list> KCCs; // k-connected components
	std::vector<adj_list> Q_subG; // Connected subgraphs that may or may not be k-connected.

	// Start by inputting all connected components.
	{
		adj_list G_kcore = get_subgraph(G, kcore_vertices_undirected(G, k)); // vertices not in the k core are definitely not in a k connected subgraph
		auto& all_CCV = all_CC_vertices_undirected(G_kcore);
		for (auto& ccv : all_CCV) {
			Q_subG.push_back(get_subgraph(G, ccv));
		}
	}

	std::cout << "k: " << k << std::endl;

	while (!Q_subG.empty()) {
		adj_list subG = Q_subG.back();
		Q_subG.pop_back();

		size_t n = subG.size();
		size_t m = 0;
		for (auto& vec : subG)
			m += vec.size();

		if (n < k + 2 || m == 0) {
			continue; // Discard very small graphs even though they might be valid k-connected components.
		}

		std::unordered_set<size_t> cut;
		bool found_cut = vc(localec, unbalancedvc, balancedvc, subG, k, 2.0 / 3.0, cut);

		std::cout << "n: " << n << "\tm: " << m << "\tcutsize: " << cut.size() << "\tcut:";
		for (size_t v : cut)
			std::cout << " " << v;
		std::cout << std::endl;

		if (found_cut) {
			for (adj_list& CC : CC_after_cut(subG, cut)) {
				Q_subG.push_back(CC);
			}	
		}
		else {
			KCCs.push_back(subG);
		}
	}

	return KCCs;
}

void preprocessing::inplace_remove_duplicate_edges(adj_list& G) {
	for (auto& vec : G) {
		sort(vec.begin(), vec.end());
		auto& it = unique(vec.begin(), vec.end());
		vec.erase(it, vec.end());
	}
}