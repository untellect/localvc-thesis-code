#pragma once

//#include "pch.h"

//#include <iostream>
//#include <fstream>
#include <chrono>
////#include <math.h>

//#include "GraphIO.h"
//#include "Graphgen.h"
#include "localvc.h"
#include "localvc2.h"
#include "Henzinger.h"



using namespace localvc;
using namespace std;
using namespace std::chrono;

void benchmark_adj(const adj_list& adjacency_list, size_t N, size_t boost, double a_coeff);
void benchmark_file(string filename, size_t N = 10, size_t boost = 10, double a_coeff = 2.0 / 3.0);
void benchmark_file2(string filename, size_t N = 10, size_t boost = 10, double a_coeff = 2.0 / 3.0, string id = "");

/* * */

void benchmark_localec(graph& g, vertex x, size_t mu, size_t k, size_t N, localec_fn localec, string id);
void benchmark_localec_multi_nu(graph& g, vertex x, size_t mu, size_t k, size_t N, localec_fn localec, string id, std::vector<double> d_i = { 1, 2, 3, 4, 6, 8, 10, 12, 14, 16, 24, 32 });
void benchmark_localec2_multi_nu(localvc2::graph& g, vertex x, size_t mu, size_t k, size_t N, localvc2::localec_fn localec, string id, std::vector<double> d_i = { 1, 2, 3, 4, 6, 8, 10, 12, 14, 16, 24, 32 });

void benchmark_globalvc(graph& g, size_t k, size_t a, size_t N, globalvc_fn globalvc, localec_fn localec, string id);

void benchmark_ud(const std::vector<std::vector<vertex>>& adjacency_list, double a_coeff, size_t N, size_t boost, globalvc_ud_fn globalvc, localec_fn localec, string id);

void benchmark_ud2(const std::vector<std::vector<vertex>>& adjacency_list, double a_coeff, size_t N, size_t boost, localec_fn localec, string id);

void benchmark_ud3(graph& g, size_t k, size_t a, size_t N, globalvc_ud_fn globalvc, localec_fn localec, string id);

void benchmark_henzinger(henzinger::henzinger_vc_fn vc, const std::vector<std::vector<vertex>>& adjacency_list, size_t N, std::string id = "", bool verbose = false);

void benchmark_VC_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adjacency_list, size_t N);

void benchmark_find_VC_undirected(VC_fn vc, localec_fn localec, globalvc_ud_fn unbalancedvc, globalvc_balanced_fn balancedvc, const std::vector<std::vector<vertex>>& adjacency_list, double a_coeff, size_t boost, size_t N, string id);

void benchmark_pair_edmonds_karp(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max);

void benchmark_pair_fordfulkerson(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max);

void benchmark_pair_fordfulkerson2(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max);

void benchmark_pair_fordfulkerson3(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs, size_t k_max);

void benchmark_pair_preflow(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs);

void benchmark_henzinger_pair(const std::vector<std::vector<vertex>>& adjacency_list, std::vector<std::pair<size_t, size_t>> s_t_pairs);

