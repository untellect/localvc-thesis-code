#include "pch.h"

#include "GraphIO.h"
#include <vector>
#include <ios>

versioned_graph::graph read_from_edgelist_undirected(std::ifstream& in) {
	return versioned_graph::graph(read_adjlist_from_edgelist_undirected(in));
}

// Assumes each edge is has only one row "a b" in the file where a > b.
// Output is birectional (both directions included)
std::vector<std::vector<versioned_graph::vertex>> read_adjlist_from_edgelist_undirected(std::ifstream& in, size_t skip_lines) {
	for(size_t i = 0; i < skip_lines; i++)
		in.ignore(10000, '\n');

	size_t n = 0;
	versioned_graph::vertex s, t;
	std::vector<std::vector<versioned_graph::vertex>> adj_list;
	while (in >> s >> t) {
		if (s >= n) {
			n = s + 1;
			adj_list.resize(n);
		}
		if (t >= n) {
			n = t + 1;
			adj_list.resize(n);
		}

		adj_list[s].push_back(t);
		adj_list[t].push_back(s);
	}

	return adj_list;
}

void write_adjlist_to_edgelist_undirected(std::ofstream& out, std::vector<std::vector<versioned_graph::vertex>>& adj_list) {
	size_t n = adj_list.size();

	for (size_t s = 0; s < n; s++) {
		for (size_t t : adj_list[s]) {
			if (s <= t) continue; // Only save edges where v > w.
			out << s << " " << t << std::endl;
		}
	}
}


versioned_graph::graph read_from_edgelist_undirected2(std::ifstream& in) {
	size_t n = 0;
	versioned_graph::vertex s, t;
	versioned_graph::graph g;
	while (in >> s >> t) {
		g.add_edge(s, t);
		g.add_edge(t, s);
	}
	return g;
}

std::vector<size_t> read_degrees_from_edgelist_undirected(std::ifstream& in, size_t skip_lines) {
	for (size_t i = 0; i < skip_lines; i++)
		in.ignore(10000, '\n');

	size_t n = 0;
	versioned_graph::vertex s, t;
	std::vector<size_t> degrees;
	while (in >> s >> t) {
		if (s >= n) {
			n = s + 1;
			if (n > 1000000000) std::cout << n << std::endl;
			degrees.resize(n);
		}
		if (t >= n) {
			n = t + 1;
			if (n > 1000000000) std::cout << n << std::endl;
			degrees.resize(n);
		}

		degrees[s]++;
		degrees[t]++;
	}

	return degrees;
}

// Does not shift vertex numbering.
std::vector<std::vector<versioned_graph::vertex>> read_subgraph_from_edgelist_undirected(std::ifstream& in, std::vector<bool>& subgraph_mask, size_t skip_lines) {
	for (size_t i = 0; i < skip_lines; i++)
		in.ignore(10000, '\n');

	size_t n = subgraph_mask.size();

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	{
		size_t v_new = 0;
		for (size_t v_old = 0; v_old < n; v_old++) {
			if (subgraph_mask[v_old]) {
				old_to_new_mapping[v_old] = v_new++;
			}
		}
	}

	versioned_graph::vertex s, t;
	std::vector<std::vector<versioned_graph::vertex>> adj_list(n);
	while (in >> s >> t) {
		if (!subgraph_mask[s] || !subgraph_mask[t]) continue; // Ignore edges not in the subgraph mask.

		adj_list[s].push_back(t);
		adj_list[t].push_back(s);
	}

	return adj_list;
}

void read_and_write_subgraph_edgelist_undirected(std::string in_path, std::string out_path, std::vector<bool>& subgraph_mask, size_t skip_lines) {
	std::ifstream in;
	in.open(in_path);
	std::ofstream out;
	out.open(out_path);

	for (size_t i = 0; i < skip_lines; i++)
		in.ignore(10000, '\n');

	size_t n = subgraph_mask.size();

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	{
		size_t v_new = 0;
		for (size_t v_old = 0; v_old < n; v_old++) {
			if (subgraph_mask[v_old]) {
				old_to_new_mapping[v_old] = v_new++;
			}
		}
	}

	for (size_t i = 0; i < skip_lines; i++)
		out << std::endl;

	size_t m = 0;
	versioned_graph::vertex s, t;
	while (in >> s >> t) {
		if (!subgraph_mask[s] || !subgraph_mask[t]) continue; // Ignore edges not in the subgraph mask.

		size_t s_new = old_to_new_mapping[s];
		size_t t_new = old_to_new_mapping[t];

		out << s_new << " " << t_new << std::endl;
		m++;
	}

	std::cout << "m: " << m << std::endl;
}

void read_edgelist_write_binary(std::string in_path, std::string out_path, size_t skip_lines) {
	std::ifstream in;
	in.open(in_path);
	std::fstream out;
	out.open(out_path, std::ios::out | std::ios::binary | std::ios::trunc);

	for (size_t i = 0; i < skip_lines; i++)
		in.ignore(10000, '\n');

	const size_t BUF_SIZE = 512;
	size_t buffer[BUF_SIZE];
	size_t i = 0, j = 0;

	while (in >> buffer[i++] >> buffer[i++]) {
		if (i < BUF_SIZE)
			j = i;
		else {
			out.write((char*)buffer, 512 * sizeof(size_t));
			i = j = 0;
		}
	}
	out.write((char*)buffer, j * sizeof(size_t)); // Leftovers after buffer runs out.
}

std::vector<std::vector<versioned_graph::vertex>> read_adjlist_from_binary_undirected(std::string in_path) {
	std::fstream in;
	in.open(in_path, std::ios::in | std::ios::binary);

	/*
	const size_t BUF_SIZE = 512; // Must be divisible by two.
	size_t buffer[BUF_SIZE];

	size_t n = 0;
	std::vector<std::vector<versioned_graph::vertex>> adj_list;
	while (in.read((char*)buffer, 512 * sizeof(size_t))) {
		for (size_t i = 0; i < BUF_SIZE;) {
			size_t s = buffer[i++];
			size_t t = buffer[i++];

			if (s >= n) {
				n = s + 1;
				while (adj_list.size() < n) adj_list.push_back({});
			}
			if (t >= n) {
				n = t + 1;
				while (adj_list.size() < n) adj_list.push_back({});
			}

			adj_list[s].push_back(t);
			adj_list[t].push_back(s);
		}
	}

	size_t remainder = (in.gcount()) / sizeof(size_t);
	for (size_t i = 1; i < remainder; i += 2) {
		size_t s = buffer[i-1];
		size_t t = buffer[i];

		if (s >= n) {
			n = s + 1;
			while (adj_list.size() < n) adj_list.push_back({});
		}
		if (t >= n) {
			n = t + 1;
			while (adj_list.size() < n) adj_list.push_back({});
		}

		adj_list[s].push_back(t);
		adj_list[t].push_back(s);
	}


	return adj_list;
	*/

	size_t n = 0;
	versioned_graph::vertex s, t;
	std::vector<std::vector<versioned_graph::vertex>> adj_list;
	while (in >> s >> t) {
		if (s >= n) {
			n = s + 1;
			adj_list.resize(n);
		}
		if (t >= n) {
			n = t + 1;
			adj_list.resize(n);
		}

		adj_list[s].push_back(t);
		adj_list[t].push_back(s);
	}

	return adj_list;
}

void write_adjlist_to_binary_undirected(std::string out_path, std::vector<std::vector<versioned_graph::vertex>> adj_list) {
	std::fstream out;
	out.open(out_path, std::ios::out | std::ios::binary | std::ios::trunc);

	size_t n = adj_list.size();
	for (size_t s = 0; s < n; s++) {
		for (size_t t : adj_list[s]) {
			if (s <= t) continue; // Only save edges where v > w.
			out << s << " " << t << std::endl;
		}
	}

	/*
	const size_t BUF_SIZE = 512;
	size_t buffer[BUF_SIZE];
	size_t i = 0, j = 0;

	size_t n = adj_list.size();
	for (size_t s = 0; s < n; s++) {
		for (size_t t : adj_list[s]) {
			if (s <= t) continue; // Only save edges where v > w.
			buffer[i] = s;
			buffer[i+1] = t;
			i += 2;

			if (i < BUF_SIZE)
				j = i;
			else {
				out.write((char*)buffer, 512 * sizeof(size_t));
				i = j = 0;
			}
		}
	}
	out.write((char*)buffer, j * sizeof(size_t)); // Leftovers after buffer runs out.
	*/
}

std::vector<size_t> read_degrees_from_binary_undirected(std::string in_path) {
	std::ifstream in;
	in.open(in_path, std::ios::in | std::ios::binary);

	size_t n = 0;
	std::vector<size_t> degrees;

	versioned_graph::vertex buffer[500];
	size_t readsize = 500 * sizeof(versioned_graph::vertex);

	while(in.read((char*)buffer, readsize)) {
		for (size_t i = 0; i < 500; i++) {
			const size_t& v = buffer[i];
			if (v >= n) {
				degrees.resize(v + 1);
			}
			degrees[v]++;
		}
	}

	size_t extras = in.gcount() / sizeof(versioned_graph::vertex);
	for (size_t i = 0; i < extras; i += 2) {
		size_t s = buffer[i];
		size_t t = buffer[i + 1];

		if (s >= n) {
			degrees.resize(s + 1);
		}
		if (t >= n) {
			degrees.resize(t + 1);
		}

		degrees[s]++;
		degrees[t]++;
	}

	return degrees;
}

void read_and_write_subgraph_binary_undirected(std::string in_path, std::string out_path, std::vector<bool>& subgraph_mask) {
	std::ifstream in;
	in.open(in_path, std::ios::in | std::ios::binary);
	std::ofstream out;
	out.open(out_path, std::ios::out | std::ios::binary | std::ios::trunc);

	size_t n = subgraph_mask.size();

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	{
		size_t v_new = 0;
		for (size_t v_old = 0; v_old < n; v_old++) {
			if (subgraph_mask[v_old]) {
				old_to_new_mapping[v_old] = v_new++;
			}
		}
	}

	size_t m = 0;
	versioned_graph::vertex s, t;
	while (in >> s >> t) {
		if (!subgraph_mask[s] || !subgraph_mask[t]) continue; // Ignore edges not in the subgraph mask.

		size_t s_new = old_to_new_mapping[s];
		size_t t_new = old_to_new_mapping[t];

		out << s_new << t_new << std::endl;
		m++;
	}

	std::cout << "m: " << m << std::endl;
}

// Does not shift vertex numbering.
std::vector<std::vector<versioned_graph::vertex>> read_subgraph_from_binary_undirected(std::string in_path, std::vector<bool>& subgraph_mask) {
	std::ifstream in;
	in.open(in_path, std::ios::in | std::ios::binary);

	size_t n = subgraph_mask.size();

	std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
	{
		size_t v_new = 0;
		for (size_t v_old = 0; v_old < n; v_old++) {
			if (subgraph_mask[v_old]) {
				old_to_new_mapping[v_old] = v_new++;
			}
		}
	}

	versioned_graph::vertex s, t;
	std::vector<std::vector<versioned_graph::vertex>> adj_list(n);
	while (in >> s >> t) {
		if (!subgraph_mask[s] || !subgraph_mask[t]) continue; // Ignore edges not in the subgraph mask.

		adj_list[s].push_back(t);
		adj_list[t].push_back(s);
	}

	return adj_list;
}

void write_degree_distribution(std::ostream& out, std::vector<std::vector<versioned_graph::vertex>>& adj_list) {
	if (adj_list.empty())
		return;

	std::vector<size_t> degrees;
	degrees.reserve(adj_list.size());
	for (auto& vec : adj_list)
		degrees.push_back(vec.size());
	sort(degrees.begin(), degrees.end());

	std::cout << "d\tN" << std::endl;

	size_t D = degrees.front();
	size_t N = 0;

	for (size_t d : degrees) {
		if (D == d) {
			N++;
		}
		else {
			out << D << "\t" << N << std::endl;
			D = d;
			N = 1;
		}
	}
	out << D << "\t" << N << std::endl;
}