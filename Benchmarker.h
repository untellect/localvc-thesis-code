#pragma once

#include "Localvc.h"
#include "localvc2.h"
#include "Henzinger.h"

#include <chrono>

struct UndirectedBenchmarker
{	
private:
	std::string graphid;
	localvc::adj_list adjacency_list;
	size_t n = 0, m = 0, d_min = 0;
public:
	static const size_t HENZ_VERSIONS = 4;
	static const size_t LOCAL1_VERSIONS = 3;
	static const size_t LOCAL2_VERSIONS = 10;

	struct res_t {
		struct localvc_res {
			std::chrono::steady_clock::duration t;
			size_t k = 0;
			size_t boost = 0;
			std::unordered_set<size_t> separator;
			bool on = false;
		};
		std::vector<localvc_res> local[LOCAL1_VERSIONS];
		std::vector<localvc_res> local2[LOCAL2_VERSIONS];

		struct henzinger_res {
			std::chrono::steady_clock::duration t;
			size_t k = 0;
			std::vector<henzinger::henzinger_subresult> subresults;
			std::unordered_set<size_t> separator;
			bool on = false;
		};
		std::vector<henzinger_res> henz[HENZ_VERSIONS];

		std::string graphid;
		size_t N = 0;
		size_t n = 0;
		size_t m = 0;
		size_t d_min = 0;
		size_t k_min = SIZE_MAX;

		size_t fg_k = 0;
		size_t fg_m = 0;
	};
	std::vector<res_t> results;

	struct localvc_cfg_s {
		localvc::VC_fn* vc = localvc::VC_undirected_edge_sampling;
		localvc::localec_fn* localec = localvc::local_edge_connectivity_v2;
		localvc::globalvc_ud_fn* unbalancedvc = localvc::vertex_connectivity_unbalanced_undirected_edge_sampling;
		localvc::globalvc_balanced_fn* balancedvc = localvc::vertex_connectivity_balanced_edge_sampling_adj_list;

		std::string name = "LOCAL";

		size_t boost = 1;
		double a_coeff = 3.0; // At least 3 or so.

		bool on = false;
	} localvc_cfg[LOCAL1_VERSIONS];

	struct localvc2_cfg_s {
		localvc2::find_VC_fn* find_vc = localvc2::find_and_improve_vertex_connectivity_undirected;
		localvc2::VC_fn* vc = localvc2::VC_undirected_edge_sampling;
		localvc2::localec_fn* localec = localvc2::local_edge_connectivity_v2;
		//localvc2::localec_fn* localec = localvc2::local_edge_connectivity_v2;
		localvc2::globalvc_ud_fn* unbalancedvc = localvc2::vertex_connectivity_unbalanced_undirected_edge_sampling;
		localvc2::globalvc_balanced_fn* balancedvc = localvc::vertex_connectivity_balanced_edge_sampling_adj_list;

		std::string name = "LOCALDEG";

		bool on = false;
	} localvc2_cfg[LOCAL2_VERSIONS];

	struct henzinger_cfg_s {
		henzinger::henzinger_vc_fn* vc = henzinger::vc_undirected_linear_random;
		bool on = false;

		std::string name = "HRG";

		//std::vector<std::vector<henzinger_res>> results;
	} henzinger_cfg[HENZ_VERSIONS];

	void setGraph(localvc::adj_list& G, std::string name);
	void updateGraphMeta();

	size_t N = 1;

	void read_undirected_graph_from_file(std::string filename, std::string graphid, bool printinfo);
	void write_undirected_graph_to_file(std::string filename, bool printinfo);

	void run_local_based(size_t ver);
	void run_local2_based(size_t ver); // only singleboost
	void run_henzinger(size_t ver);

	enum print_level {
		QUIET = 0,
		NORMAL = 1,
		EXTRA = 2,
		SUMMARY = 3,
		CONDENSED_SUMMARY = 4,
		EXTRA_CONDENSED_SUMMARY = 5,
		EXTRA_CONDENSED_SUMMARY_CSV = 6,
	} print_level = NORMAL;

	// Print level 0: quiet
	// Print level 1: normal
	// Print level 2: extra (henzinger)
	// Print level 3: summary
	// Print level 4: one line: all algorithms for one graph type

	void print_header(std::ostream& out);
	void output(std::ostream& out); // Full output.

	void clear_results();
	void run();

	UndirectedBenchmarker() {
		// Henzinger, Rao & Gabow
		if (HENZ_VERSIONS > 0) {
			henzinger_cfg[0].vc = henzinger::vc_undirected_doubling2_random;
			henzinger_cfg[0].name = "HRG";
		}
		if (HENZ_VERSIONS > 1) {
			henzinger_cfg[1].vc = henzinger::vc_undirected_linear_random;
			henzinger_cfg[1].name = "HRGLIN";
		}
		if (HENZ_VERSIONS > 2) {
			henzinger_cfg[2].vc = henzinger::vc_undirected_linear_unsparsified_random;
			henzinger_cfg[2].name = "HRGLINUSP";
		}
		if (HENZ_VERSIONS > 3) {
			henzinger_cfg[3].vc = henzinger::vc2_undirected_doubling2_random;
			henzinger_cfg[3].name = "HRG2";
		}

		// Local (graph implementation 1)
		if (LOCAL1_VERSIONS > 0) {
			localvc_cfg[0].name = "LOCAL1";
			localvc_cfg[0].localec = localvc::local_edge_connectivity_v2;
		}
		if (LOCAL1_VERSIONS > 1) {
			localvc_cfg[1].name = "LOCAL1DMIN";
			localvc_cfg[1].localec = localvc::local_edge_connectivity_v2;
			localvc_cfg[1].vc = localvc::VC_undirected_edge_sampling_trivial_separately;
		}
		if (LOCAL1_VERSIONS > 2) {
			localvc_cfg[2].name = "PAGE10";
			localvc_cfg[2].localec = localvc::local_edge_connectivity_v8;
		}

		// Local (graph implementation 2)
		if (LOCAL2_VERSIONS > 0) {
			localvc2_cfg[0].name = "LOCAL2";
			localvc2_cfg[0].localec = localvc2::local_edge_connectivity_v2;
		}
		if (LOCAL2_VERSIONS > 1) {
			localvc2_cfg[1].name = "LOCAL2DMIN";
			localvc2_cfg[1].localec = localvc2::local_edge_connectivity_v2;
			localvc2_cfg[1].vc = localvc2::VC_undirected_edge_sampling_trivial_separately;
		}
		if (LOCAL2_VERSIONS > 2) {
			localvc2_cfg[2].name = "LOCAL2DEG";
			localvc2_cfg[2].localec = localvc2::local_edge_connectivity_degreecount;
		}
		if (LOCAL2_VERSIONS > 3) {
			localvc2_cfg[3].name = "LOCAL+";
			localvc2_cfg[3].localec = localvc2::local_edge_connectivity_degreecount;
			localvc2_cfg[3].vc = localvc2::VC_undirected_edge_sampling_trivial_separately;
		}
		if (LOCAL2_VERSIONS > 4) {
			// THIS VERSION DOES NOT WORK!
			localvc2_cfg[4].name = "LOCAL2MARKDEG";
			localvc2_cfg[4].localec = localvc2::local_edge_connectivity_marking_degreecount;
			//localvc2_cfg[4].vc = localvc2::VC_undirected_edge_sampling_trivial_separately;
			//localvc2_cfg[4].a_coeff = 20.0; // Marking version becomes quite slow when approaching the size of the graph.
		}
		if (LOCAL2_VERSIONS > 5) {
			localvc2_cfg[5].name = "LOCAL2MARK2DEG";
			localvc2_cfg[5].localec = localvc2::local_edge_connectivity_marking2_degreecount;
			//localvc2_cfg[5].vc = localvc2::VC_undirected_edge_sampling_trivial_separately;
		}
		if (LOCAL2_VERSIONS > 6) {
			localvc2_cfg[6].name = "LOCALh+";
			localvc2_cfg[6].localec = localvc2::local_edge_connectivity_marking2_degreecount;
			localvc2_cfg[6].vc = localvc2::VC_undirected_edge_sampling_trivial_separately;
		}
		if (LOCAL2_VERSIONS > 7) {
			localvc2_cfg[7].name = "NOLOCAL";
			localvc2_cfg[7].unbalancedvc = localvc2::vertex_connectivity_unbalanced_skip;
		}
		if (LOCAL2_VERSIONS > 8) {
			localvc2_cfg[8].name = "LOCAL";
			localvc2_cfg[8].localec = localvc2::local_edge_connectivity_minors;
			localvc2_cfg[8].vc = localvc2::VC_undirected_edge_sampling_trivial_separately;
		}
		if (LOCAL2_VERSIONS > 9) {
			localvc2_cfg[9].name = "LOCAL-";
			localvc2_cfg[9].localec = localvc2::local_edge_connectivity_v1;
		}
	}
};

