#pragma once

#include <fstream>
#include "Graph.h"

versioned_graph::graph read_from_edgelist_undirected(std::ifstream& in);
versioned_graph::graph read_from_edgelist_undirected2(std::ifstream& in);

void write_adjlist_to_edgelist_undirected(std::ofstream& out, std::vector<std::vector<versioned_graph::vertex>>& adj_list);

std::vector<std::vector<versioned_graph::vertex>> read_adjlist_from_edgelist_undirected(std::ifstream& in, size_t skip_lines = 0);

// For large k-core
std::vector<size_t> read_degrees_from_edgelist_undirected(std::ifstream& in, size_t skip_lines = 0);
std::vector<std::vector<versioned_graph::vertex>> read_subgraph_from_edgelist_undirected(std::ifstream& in, std::vector<bool>& subgraph_mask, size_t skip_lines = 0);
void read_and_write_subgraph_edgelist_undirected(std::string in_path, std::string out_path, std::vector<bool>& subgraph_mask, size_t skip_lines = 0);

void read_edgelist_write_binary(std::string in_path, std::string out_path, size_t skip_lines = 0);
std::vector<std::vector<versioned_graph::vertex>> read_adjlist_from_binary_undirected(std::string in_path);
void write_adjlist_to_binary_undirected(std::string out_path, std::vector<std::vector<versioned_graph::vertex>> adj_list);
std::vector<size_t> read_degrees_from_binary_undirected(std::string in_path);
void read_and_write_subgraph_binary_undirected(std::string in_path, std::string out_path, std::vector<bool>& subgraph_mask);
std::vector<std::vector<versioned_graph::vertex>> read_subgraph_from_binary_undirected(std::string in_path, std::vector<bool>& subgraph_mask);

// Special
void write_degree_distribution(std::ostream& out, std::vector<std::vector<versioned_graph::vertex>>& adj_list);
