#include "pch.h"

#include "Graph2.h"

#include <cassert>

namespace resettable_graph {
	void graph::add_edge(vertex s, vertex t) {
		if (s >= adj.size()) {
			adj.resize(s + 1);

			internal_parent.resize(s + 1);
			is_visited.resize(s + 1);
			is_counted.resize(s + 1);
			uncounted_edges.resize(s + 1);
		}
		if (t >= adj.size()) {
			adj.resize(t + 1);

			internal_parent.resize(t + 1);
			is_visited.resize(t + 1);
			is_counted.resize(t + 1);
			uncounted_edges.resize(t + 1);
		}
		adj[s].push_back(t);
	}

	const_inc_list& graph::operator[](index i) const {
		return adj[i];
	}

	inc_list_iterator graph::begin() const {
		return adj.begin();
	}

	inc_list_iterator graph::end() const {
		return adj.end();
	}

	// number of *vertices*
	size_t graph::size() const {
		return adj.size();
	}

	/*
	 * Prints the current state of the graph with the adjacency list of each vertex on its own line.
	 */
	std::ostream& operator<<(std::ostream& os, const graph& g) {
		for(vertex s = 0; s < g.adj.size(); ++s) {
			os << s << ":";
			for(vertex t : g.adj[s]) {
				os << " " << t;
			}
			os << std::endl;
		}
		return os;
	}


	void graph::init_count(vertex w) {
		counted_vertices.push_back(w);
		uncounted_edges[w] = adj[w].size();
		is_counted[w] = true;
	}

	void graph::new_search() {
		for (vertex v : visited_vertices) {
			is_visited[v] = false;
		}
		visited_vertices.clear();
	}
	//bool graph::visited(vertex v, version ver) const {
	//	return last_visited[v] == ver;
	//}
	//version graph::current_search() const {
	//	return search_ver;
	//}
	// No guarantee that ver is the current search version.
	void graph::visit(vertex v, vertex from, index from_i) {
		is_visited[v] = true;
		visited_vertices.push_back(v);
		internal_parent[v] = { from, from_i };

		//last_visited[v] = ver;
		//latest_parent[v] = { from, from_i };
	}
	void graph::visit(vertex v, const internal_location& from) {
		is_visited[v] = true;
		visited_vertices.push_back(v);
		internal_parent[v] = from;

		//last_visited[v] = ver;
		//latest_parent[v] = from;
	}
	// Should check first that the vertex is visited in the current search.s
	vertex graph::parent(vertex v) {
		return internal_parent[v].v;
		//return latest_parent[v].v;
	}

	void graph::reverse_source_to_vertex(vertex t) {
		if (internal_parent[t].v == t) {
			return; // Do nothing if t is not visited or the root of the current search.
		}

		size_t path_len = 0;
		{
			vertex v = t;
			while (internal_parent[v].v != v) {
				path_len++;
				v = internal_parent[v].v;
			}
		}

		std::vector<internal_location> reversed_path;
		reversed_path.reserve(path_len + 1);
		reversed_path.push_back({ t, 0 });

		vertex s = internal_parent[t].v;
		adj[t].push_back(s); // add (target -> source)
		while (internal_parent[s].v != s) {
			vertex parent = t;
			t = s;
			s = internal_parent[t].v;
			index i = internal_parent[parent].i;

			adj[t][i] = s; // replace (target -> parent) with (target -> source)

			reversed_path.push_back({ t, i });
		}
		index i = internal_parent[t].i;
		adj[s][i] = adj[s].back(); // swap-and-pop (source -> target)
		adj[s].pop_back();
		reversed_path.push_back({ s, i });
		reversal_history.push_back(reversed_path);
	}

	// undo all reversals
	void graph::reset() {
		while (!reversal_history.empty()) {
			const std::vector<internal_location>& reversed_path = reversal_history.back();
			if (reversed_path.size() == 1) {
				reversal_history.pop_back();
				continue;
			}

			index i = reversed_path.size() - 1;
			vertex t =  reversed_path[reversed_path.size() - 1].v;
			index i_t = reversed_path[reversed_path.size() - 1].i;
			vertex s =  reversed_path[reversed_path.size() - 2].v;
			index i_s = reversed_path[reversed_path.size() - 2].i;

			adj[t].push_back(s);					// add (target -> source)
			std::swap(adj[t][i_t], adj[t].back());	// Swap the edge back into its original position.

			for (index i = reversed_path.size() - 2; i > 0; --i) {
				const internal_location& s_loc = reversed_path[i - 1];
				vertex parent = t;
				t = s;
				s = s_loc.v;
				i_t = i_s;
				i_s = s_loc.i;

				assert(adj[t][i_t] == parent);	// (target -> parent) should be the i_t:th edge of source.
				adj[t][i_t] = s;				// Replace (target -> parent) with (target -> source)
			}

			assert(adj[s].back() == t); // (source -> target) should be the last edge of source.
			adj[s].pop_back();			// remove (source -> target)

			reversal_history.pop_back();
		}
		new_search();
		uncount();
	}

	void graph::uncount() {
		for (vertex v : counted_vertices) {
			is_counted[v] = false;
		}
		counted_vertices.clear();
	}
}
