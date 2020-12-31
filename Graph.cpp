#include "pch.h"
#include "Graph.h"

namespace versioned_graph {

	/*
	 * Creates a graph with 0 vertices and 0 edges.
	 */
	graph::graph() : ver(1), dfs_v(1) {
		data.push_back({ 0, 0 });
		special_index.push_back(0);
	}

	/*
	 * Adds one vertex to the graph. It has no edges.
	 */
	void graph::add_vertex() {
		vertex v = special_index.size() - 1;
		size_t i = data.size();
		data.push_back({ v, v });
		special_index.push_back(i);
		dfs_parents.push_back({});
	}

	/*
	 * Adds an edge (s -> t) to the graph.
	 * If either vertex is not in the graph we add vertices until they are.
	 * If s is not the last vertex that has edges some adjacency lists may change order.
	 * O(V) in the worst case.
	 */
	void graph::add_edge(vertex s, vertex t) {
		size_t n = size_vertices();
		if (n <= s || n <= t) {
			while (n <= s || n <= t) {
				add_vertex();
				++n;
			}
		}
		data.push_back(data.back());
		++special_index.back();
		size_t i = data.size() - 2;
		vertex v = special_index.size() - 2;
		// invariant: i is the index of a free slot at the end of the adjacency list of v.
		while (s != v) {
			data[i] = data[special_index[v] + 1];					// Move first vertex from v to the free slot.
			data[special_index[v] + 1] = data[special_index[v]];	// Move special vertex of v forward one space.
			i = special_index[v];									// The old location of special index v is now free.
			++special_index[v];										// The special index of v is moved forward.
			--v;													// We are in the adjacency list of (v-1).
		}
		data[i] = node(s, t);
	}

	/*
	 * Adds an edge (s -> t) to the graph.
	 * If either vertex is not in the graph we add vertices until they are.
	 * The edge is appended to the end of the adjacency list of s. The order of existing edges is not changed.
	 * O(E+V) in the worst case.
	 * Identical to the unstable one in the best case.
	 */
	void graph::add_edge_stable(vertex s, vertex t) {
		size_t n = size_vertices();
		if (n <= s || n <= t) {
			while (n <= s || n <= t) {
				add_vertex();
				++n;
			}
		}

		data.insert(data.begin() + special_index[s + 1], { s, t });
		for (vertex v = s + 1; v < special_index.size(); ++v) {
			++special_index[v];
		}
	}

	/*
	 * MUST be edge (a, b) where a is the size of the graph (vertex that is not in the graph yet).
	 * MUST call add_vertex afterwards.
	 */
	void graph::add_edge_unsafe(vertex s, vertex t) {
		data.push_back({ s, t });
	}

	/*
	 * Reverses the edge pointed to by the edge iterator.
	 * Moves it from the adjacency list of the source to the end of the adjacency list of the target.
	 * Reverses the direction for the underlying node.
	 */
	void graph::reverse_edge(graph::edge_iterator& eit) {
		reverse_edge(eit.it);
	}

	/*
	 * Reverses the path from the target of the edge to the source of the DFS tree.
	 * Reverses the edge pointed to by the edge iterator, then its dfs parent and so on until we reach the source.
	 */
	void graph::reverse_path(edge_iterator& eit) {
		//std::cout << "Reversing " << eit.get_source() << " -> " << eit.get_target() << std::endl;
		//std::cout << "Reversing ";
		//std::cout << eit.get_target();
		vertex s = eit.get_source();
		_iterator it = eit.it;
		while (dfs_parents[s].ver == dfs_v && it->s != it->t) {
			//std::cout << " <- " << it->get_source(ver);
			reverse_edge(it);
			it = dfs_parents[s].parent;
			s = it->get_source(ver);
		}
		//std::cout << " : " << it->get_source(ver);
		//std::cout << std::endl;
	}

	/*
	void graph::reverse_path(vertex s) {
		//std::cout << "Reversing " << eit.get_source() << " -> " << eit.get_target() << std::endl;
		//std::cout << "Reversing ";
		//std::cout << eit.get_target();
		_iterator it = dfs_parents[s].parent;
		while (dfs_parents[s].ver == dfs_v && it->s != it->t) {
			//std::cout << " <- " << eit.get_source();
			reverse_edge(it);
			it = dfs_parents[s].parent;
			s = it->get_source(ver);
		}
		//std::cout << std::endl;
	}
	*/

	/*
	 * Creates an edge iterator to the first edge in the adjacency list of s.
	 */
	graph::edge_iterator graph::begin_of(vertex s) {
#ifdef ENABLE_MARKING
		return ++edge_iterator(data.begin() + special_index[s], ver, mark_v);
#else
		return ++edge_iterator(data.begin() + special_index[s], ver);
#endif
	}
	const graph::edge_iterator graph::begin_of(vertex s) const {
#ifdef ENABLE_MARKING
		return edge_iterator(data.begin() + special_index[s], ver, mark_v) + 1;
#else
		return edge_iterator(data.begin() + special_index[s], ver) + 1;
#endif
	}
	/*
	 * Creates an edge iterator one-past-the-end of the adjacency list of s.
	 */
	graph::edge_iterator graph::end_of(vertex s) {
#ifdef ENABLE_MARKING
		return edge_iterator(data.begin() + special_index[s + 1], ver, mark_v);
#else
		return edge_iterator(data.begin() + special_index[s + 1], ver);
#endif
	}
	const graph::edge_iterator graph::end_of(vertex s) const {
#ifdef ENABLE_MARKING
		return edge_iterator(data.begin() + special_index[s + 1], ver, mark_v);
#else
		return edge_iterator(data.begin() + special_index[s + 1], ver);
#endif
	}

	/*
	 * Creates an edge iterator to the first edge in the adjacency list of s.
	 * Invalidates the previous DFS tree.
	 * Sets s as the source vertex of the DFS.
	 */
	graph::edge_iterator graph::new_dfs(vertex s) {
		++dfs_v;
		_iterator end_it = data.begin() + special_index[s + 1];
		dfs_parents[s].ver = dfs_v;     // mark v visited.
		dfs_parents[s].parent = end_it; // mark v source.
		return begin_of(s);
	}

	/*
	 * Advances the DFS.
	 * Sets the edge iterator to the next edge in DFS order. (the same edge iterator is also returned)
	 *
	 * Let eit be the edge (s -> t) (before calling this function)
	 * If t is unvisited the DFS moves to the first edge of its adjacency list (and sets s as the parent of t and its outgoing edges)
	 *   If the adjacency list of t was empty, it is backtracked from back to (s -> t) and in the next iteration of the while loop t is not unvisited.
	 *	 Otherwise we have a valid edge.
	 * If t is visited we increment the edge iterator to move to the next outgoing edge of s.
	 *   If t was the last outgoing edge of s we backtrack to the parent (p -> s) of s.
	 *     If p != s we backtracked to some edge from where we advanced to the vertex s. We keep trying to advance the DFS iterator from here.
	 *     Otherwise we have reached the end iterator of the source vertex, which is the end iterator for the DFS.
	 *   Otherwise we have a valid edge.
	 */
	graph::edge_iterator& graph::advance_dfs(edge_iterator& eit) {
		while (eit.it->s != eit.it->t) { // dfs is not over
			// If the current target is unvisited, visit it.
			vertex v = eit.get_target();

			if (dfs_parents[v].ver != dfs_v) { // child is unvisited
				dfs_parents[v].ver = dfs_v;
				dfs_parents[v].parent = eit.it;
				eit.it = next(data.begin() + special_index[v], ver);
			}
			// Otherwise increment the current edge iterator (unless at the end of an adjacency list)
			else {
				++eit;
			}
			// If the edge is valid we are done.
			if (eit.it->s != eit.it->t) {
				break;
			}
			// If the edge is invalid but has a parent (not a self loop), pop the current vertex off the stack by going to the parent.
			else {
				backtrack_dfs(eit);
			}
			// If the edge is invalid and has no valid parent, the backtracking will do nothing and the loop ends, otherwise continue.
		}
		return eit;
	}

	/*
	 * Makes the edge iterator point to its parent edge in the DFS tree, if it has a valid parent.
	 */
	graph::edge_iterator& graph::backtrack_dfs(edge_iterator& eit) {
		vertex s = eit.get_source();
		if (dfs_parents[s].ver == dfs_v) {
			eit.it = dfs_parents[s].parent;
		}
		return eit;
	}

	/*
	 * The number of vertices in the graph.
	 */
	size_t graph::size_vertices() const {
		return special_index.size() - 1;
	}
	/*
	 * The number of edges in the graph.
	 */
	size_t graph::size_edges() const {
		return data.size() - special_index.size();
	}

	/*
	 * Minimum degree of the graph.
	 */
	size_t graph::min_degree() const{
		size_t n = size_vertices();
		size_t d_min = SIZE_MAX;
		for (vertex v = 0; v < n; v++) {
			size_t d_v = special_index[v + 1] - special_index[v] - 1;
			if (d_v < d_min)
				d_min = d_v;
		}
		return d_min;
	}

#ifdef _DEBUG
	/*
	 * A debug print function that ignores all changes (prints a reset graph without an actual reset)
	 */
	void graph::dumb_print(std::ostream& os) const {
		size_t n = size_vertices();
		for (vertex s = 0; s < n; ++s) {
			os << s << ":";
			auto it = data.begin() + special_index[s] + 1;
			auto end = data.begin() + special_index[s + 1];
			for (; it != end; ++it) {
				os << " " << it->t;
			}
			os << std::endl;
		}
	}
	/*
	 * A debug print function that dumps the contents of the data vector (source and target vertices only)
	 */
	void graph::dump_data(std::ostream& os) const {
		for (const node& n : data)
			os << "(" << n.s << "," << n.t << ")";
	}
	/*
	 * A debug print function that dumps the contents of the special index vector (indices of all special nodes at adjacency list endpoints)
	 */
	void graph::dump_special(std::ostream& os) const {
		for (const size_t i : special_index)
			os << "(" << i << ")";
	}
#endif

	/*
	 * Prints the current state of the graph with the adjacency list of each vertex on its own line.
	 */
	std::ostream& operator<<(std::ostream& os, const graph& g) {
		size_t n = g.size_vertices();
		for (vertex s = 0; s < g.size_vertices(); ++s) {
			os << s << ":";
			graph::edge_iterator it = g.begin_of(s);
			graph::edge_iterator end = g.end_of(s);
			for (; it != end; ++it) {
				os << " " << it.get_target();
			}
			os << std::endl;
		}
		return os;
	}

	void graph::reset() {
		++ver;
	}

#ifdef ENABLE_MARKING
	void graph::unmark() {
		++mark_v;
	}
#endif
}