#pragma once

#include <vector>

namespace resettable_graph {
	typedef size_t version_type;
	typedef size_t vertex;
	typedef size_t index;
	typedef size_t version;
	typedef const std::vector<vertex> const_inc_list;
	typedef std::vector<std::vector<vertex>>::const_iterator inc_list_iterator;

	class graph {
	public:
		struct internal_location {
			vertex v = 0;
			index i = 0;
		};
	private:
#ifdef _DEBUG
	protected:
#endif

		//std::vector<version> last_visited; // In which search versin was this vertex last visited?
		//std::vector<internal_location> latest_parent; // What was its parent vertex in the above version?
		//size_t search_ver = 1; // What is the current search version?

		std::vector<std::vector<internal_location>> reversal_history; // Contains information on all reversed paths

		std::vector<std::vector<vertex>> adj;
	public:
		// For page 10 version implementation.
		std::vector<size_t> uncounted_edges; // for page 10 degree counting variation
		std::vector<vertex> counted_vertices; // vertices that have some (possibly all) counted edges.
		std::vector<bool> is_counted;         // has some (possibly all) counted edges.

		std::vector<bool> is_visited;
		std::vector<vertex> visited_vertices;
		std::vector<internal_location> internal_parent;

		graph() {
			// Trivial constructor for empty graph.
		} 
		template<
			class VertexIterIterable,
			class VertexIterable = typename std::iterator_traits<typename VertexIterIterable::iterator>::value_type,
			class T = typename std::iterator_traits<typename VertexIterable::iterator>::value_type,
			typename = std::enable_if_t<std::is_same<T, vertex>::value, void>
		>
		graph(const VertexIterIterable& from) : graph() {
			vertex s = 0;
			for (VertexIterable vit : from) {
				//last_visited.push_back(0);
				//latest_parent.push_back({ 0, 0 });
				adj.push_back({});
				internal_parent.push_back({ 0, 0 });
				is_visited.push_back(false);
				is_counted.push_back(false);
				uncounted_edges.push_back(0); // Initialised when encountered anyway.

				for (vertex t : vit) {
					adj[s].push_back(t);
				}
				++s;
			}
		}

		void add_edge(vertex s, vertex t);

		const_inc_list& operator[](index i) const;
		inc_list_iterator begin() const;
		inc_list_iterator end() const;
		size_t size() const; // number of *vertices*

		friend std::ostream& operator<<(std::ostream& os, const graph& g);

		void init_count(vertex w);

		void new_search(); // Returns new dfs/bfs version.
		void visit(vertex v, const internal_location& from);
		void visit(vertex v, vertex from, index from_i);
		vertex parent(vertex v);
		//internal_location internal_parent(vertex v);

		void reverse_source_to_vertex(vertex t);
		void reset(); // undo all reversals
		void uncount(); // reset degree counting
	};
}
