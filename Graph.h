#pragma once

#include <vector>
#include <ostream>

#define ENABLE_MARKING

namespace versioned_graph {
	typedef size_t version_type;
	typedef size_t vertex;

	class graph {
	public:
		class edge_iterator;
#ifndef _DEBUG
	protected:
#endif
		struct node;
		typedef std::vector<node>::iterator _iterator;
		struct node {
			vertex s = -1;
			vertex t = -1;
			version_type reversed_v = 0; // reversed if this is current.
#ifdef ENABLE_MARKING
			version_type marked_v = 0; // When was this edge last marked?
			inline bool is_marked(version_type ver) const { return ver == marked_v; }
			inline void mark(version_type ver) { marked_v = ver; }
#endif

			_iterator prev;
			version_type prev_v = 0;
			_iterator next;
			version_type next_v = 0;

			node() {}
			node(vertex s, vertex t) : s(s), t(t) {}

			inline vertex get_source(version_type ver) const { return (reversed_v == ver) ? t : s; }
			inline vertex get_target(version_type ver) const { return (reversed_v == ver) ? s : t; }
			inline void reverse_direction(version_type ver) {
				if (reversed_v == ver) {
					reversed_v = 0;
				}
				else {
					reversed_v = ver;
				}
			}
		}; // struct node
		struct dfs_node {
			_iterator parent;
			version_type ver = 0;
		};

		mutable std::vector<node> data; // Mutable to handle const edge_iterators more easily. Be careful.
		std::vector<size_t> special_index;
		version_type ver;
		std::vector<dfs_node> dfs_parents;
		version_type dfs_v;
		version_type mark_v;

		inline _iterator next(const graph::_iterator& it, const version_type& ver) {
			if (it->next_v == ver)
				return it->next;
			else
				return it + 1;
		}
		inline _iterator prev(const graph::_iterator& it, const version_type& ver) {
			if (it->prev_v == ver)
				return it->prev;
			else
				return it - 1;
		}

		inline void reverse_edge(_iterator& it) {
			// Remove the edge from its adjacency list.
			_iterator next_it = next(it, ver);
			_iterator prev_it = prev(it, ver);

			next_it->prev = prev_it;
			prev_it->next = next_it;
			next_it->prev_v = prev_it->next_v = ver;

			// Connect the edge to the target adjacency list.
			_iterator end_it = data.begin() + special_index[it->get_target(ver) + 1];
			_iterator last_it = prev(end_it, ver);
			it->next = end_it;
			it->prev = last_it;
			end_it->prev = it;
			last_it->next = it;
			it->next_v = it->prev_v = end_it->prev_v = last_it->next_v = ver;

			// Reverse the edge node.
			it->reverse_direction(ver);
		}
	public:

		graph();
		// A templated constructor from a generic adjacency list interface. e.g. vector<vector<vertex>>
		template<
			class VertexIterIterable,
			class VertexIterable = typename std::iterator_traits<typename VertexIterIterable::iterator>::value_type,
			class T = typename std::iterator_traits<typename VertexIterable::iterator>::value_type,
			typename = std::enable_if_t<std::is_same<T, vertex>::value, void>
		>
			graph(const VertexIterIterable& from) : graph() {
			if (from.begin() == from.end())
				return; // Avoid adding that one vertex at the end.

			vertex s = 0;
			vertex max = 0;
			for (const VertexIterable& alist : from) {
				for (const vertex t : alist) {
					if (t > max) max = t;
					data.push_back(node(s, t));
				}
				special_index.push_back(data.size());
				dfs_parents.push_back({});
				data.push_back(node(s, s));
				++s;
			}
			while (max + 2 > special_index.size()) {
				add_vertex();
			}
		}
		void add_vertex();
		void add_edge(vertex s, vertex t);
		void add_edge_stable(vertex s, vertex t);
		void add_edge_unsafe(vertex s, vertex t);

		void reverse_edge(edge_iterator& eit);
		void reverse_path(edge_iterator& eit); // DFS path to source.
		//void reverse_path(vertex v); // DFS path to source.

		edge_iterator begin_of(vertex s);
		const edge_iterator begin_of(vertex s) const; // Do I need this?
		edge_iterator end_of(vertex s);
		const edge_iterator end_of(vertex s) const; // Do I need this?
		edge_iterator new_dfs(vertex s);
		edge_iterator& advance_dfs(edge_iterator& eit);
		edge_iterator& backtrack_dfs(edge_iterator& eit);

		size_t size_vertices() const;
		size_t size_edges() const;
		size_t min_degree() const;

#ifdef _DEBUG
		void dumb_print(std::ostream& os) const;		// debug
		void dump_data(std::ostream& os) const;			// debug
		void dump_special(std::ostream& os) const;		// debug
#endif
		friend std::ostream& operator<<(std::ostream& os, const graph& g);

		void reset();
		void unmark();
	};

	class graph::edge_iterator {
#ifdef _DEBUG
	public:
#endif
		_iterator it;

		// Because of this, using iterators to old versions causes undefined behavior.
		version_type ver;
#ifdef ENABLE_MARKING
		version_type mark_v;

		edge_iterator(_iterator _it, version_type _ver, version_type _mark_v = 0) : it(_it), ver(_ver), mark_v(_mark_v) {}
#else
		edge_iterator(_iterator _it, version_type _ver) : it(_it), ver(_ver) {}
#endif
	
		void setNext(_iterator target);
		void setPrev(_iterator target);

		inline graph::_iterator next(const graph::_iterator& it, version_type ver) {
			if (it->next_v == ver)
				return it->next;
			else
				return it + 1;
		}
		inline graph::_iterator prev(const graph::_iterator& it, version_type ver) {
			if (it->prev_v == ver)
				return it->prev;
			else
				return it - 1;
		}
	public:
		friend class graph;
		edge_iterator& operator++();
		edge_iterator& operator--();
		edge_iterator operator+(size_t delta);
		const edge_iterator operator+(size_t delta) const;
		edge_iterator operator-(size_t delta);
		const edge_iterator operator-(size_t delta) const;
		bool operator==(const edge_iterator& other) const;
		bool operator!=(const edge_iterator& other) const;
		vertex get_source() const;
		vertex get_target() const;
		bool is_edge() const;

		bool is_marked() const; // returns whether it was previously marked
		void mark();
	};
}
