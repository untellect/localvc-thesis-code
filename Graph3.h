#pragma once

#include <vector>
#include <unordered_set>

namespace partial_graph {

	typedef size_t vertex;

	class graph {

	public:
		/* is this cleaner?
		struct vertex_info {
			bool copied = false;
			bool visited = false;

			vertex parent_v = 0;
			size_t parent_i = 0;
		};
		*/

		// Original graph and partial copy
		const std::vector<std::vector<vertex>> orig;
		std::vector<std::vector<vertex>> part;

		// Current edge indices
		std::vector<size_t> orig_i; // What has been copied?
		std::vector<size_t> part_i; // Revisiting edge number part_i[v] in part[v].

		// Which vertices participate in the partial copy?
		std::vector<vertex> copied_vertices;
		std::vector<bool> copied;

		// Which vertices have been visited in the current DFS?
		std::vector<vertex> visited_vertices;
		std::vector<bool> visited;
		void visit(vertex w) {
			visited_vertices.push_back(w);
			visited[w] = true;
			part_i[w] = 0; // Restart vertex revisiting pointer the first time we see it in a DFS.
			if (!copied_vertices[w]) {
				copied_vertices.push_back(w);
				copied[w] = true;
			}
		}
		// Which index in visited_vertices are we revisiting?
		size_t revisit_i = 0; // revisiting v = visited_vertices[revisit_i], edge number part_i[v] in part[v]

		// Which vertex and what index in its adjacency list is the parent edge for this vertex in this DFS?
		std::vector<vertex> parent_v;
		std::vector<size_t> parent_i;


		template<
			class VertexIterIterable,
			class VertexIterable = typename std::iterator_traits<typename VertexIterIterable::iterator>::value_type,
			class T = typename std::iterator_traits<typename VertexIterable::iterator>::value_type,
			typename = std::enable_if_t<std::is_same<T, vertex>::value, void>
		>
		graph(const VertexIterIterable& from) : graph() {
			for (VertexIterable vit : from) {
				orig.emplace_back(vit.begin(), vit.end());
			}
			size_t n = orig.size();
			
			part.resize(n);
			orig_i.resize(n);
			part_i.resize(n);
			copied.resize(n);
			visited.resize(n);
			parent_v.resize(n);
			parent_i.resize(n);
			for (size_t v = 0; i < n; i++) {
				part[v].clear();
				orig_i[v] = 0;
				part_i[v] = 0;
				copied[v] = false;
				visited[v] = false;
			}
		}

		void new_search(vertex s) {
			// Unvisit vertices.
			for (const vertex& v : visited_vertices) {
				visited[v] = false;
			}
			visited_vertices.clear();

			// Visit source
			visit(s);

			// Reset revisiting
			revisit_i = 0;
		}
		void reset() {
			// Uncopy adjacecency lists.
			for (const vertex& v : copied_vertices) {
				copied[v] = false;
				part[v].clear();
			}
			copied_vertices.clear();

			// Unvisit vertices.
			for (const vertex& v : visited_vertices) {
				visited[v] = false;
			}
			visited_vertices.clear();
		}


	};
}