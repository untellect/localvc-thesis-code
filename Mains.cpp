#include "pch.h"

#define VERBOSE_PRINTING

//#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>
#include <random>

#include "Util.h"
#include "Sparse.h"
#include "GraphIO.h"
#include "Localec-templated.h"

#include "Mains.h"
#include "Benchmark.h"
#include "Benchmarker.h"
#include "preprocessing.h"

using namespace localvc;
using namespace std;
using namespace std::chrono;

namespace {
	//string input_dir = "C:/Users/Public/Documents/";
	string input_dir = "./DATA/";
	//string output_dir = "C:/Users/work/Documents/results/14/";
	string output_dir = "./OUTPUT/";

	void configure_bm(UndirectedBenchmarker& bm) {
		bm.N = 5;

		bm.localvc2_cfg[3].on = true; 	// LOCAL+
		bm.localvc2_cfg[6].on = true; 	// LOCALh+
		bm.localvc2_cfg[8].on = true; 	// LOCAL
		bm.henzinger_cfg[0].on = true; 	// HRG

		//bm.localvc2_cfg[0].on = true;		// LOCAL2
		//bm.localvc2_cfg[9].on = true;		// LOCAL-
		//bm.localvc2_cfg[7].on = true;		// NOLOCAL

		//bm.localvc_cfg[0].on = true;		// LOCAL1
		//bm.localvc_cfg[1].on = true;		// LOCAL1DMIN

		//bm.localvc2_cfg[1].on = true;		// LOCAL2DMIN
		//bm.localvc2_cfg[2].on = true;		// LOCAL2DEG

		//bm.localvc2_cfg[4].on = true;		// LOCAL2MARKDEG // Shitty version.
		//bm.localvc2_cfg[5].on = true;		// LOCAL2MARK2DEG
	}

	static const bool file_output = true;
}

void main5() {
	struct LSNK {
		size_t L, S, N, k;
	};

	struct parameters {
		string name = "";
		vector<LSNK> lsrk;
		bool print_header = true;
	};

	std::vector<parameters> parameter_sets;

	parameters p5_4 = { "5-4" };
	for (size_t n = 50; n < 500; n += 25) {
		p5_4.lsrk.push_back({ 5, 4, n, 60 });
	}
	for (size_t n = 500; n <= 1000; n += 50) {
		p5_4.lsrk.push_back({ 5, 4, n, 60 });
	}

	parameters p5_7 = { "5-7" };
	for (size_t n = 50; n < 500; n += 25) {
		p5_7.lsrk.push_back({ 5, 7, n, 60 });
	}
	for (size_t n = 500; n <= 1000; n += 50) {
		p5_7.lsrk.push_back({ 5, 7, n, 60 });
	}

	parameters p5_8 = { "5-8" };
	for (size_t n = 50; n < 1000; n += 50) {
		p5_8.lsrk.push_back({ 5, 8, n, 60 });
	}
	for (size_t n = 50; n <= 2000; n += 100) {
		p5_8.lsrk.push_back({ 5, 8, n, 60 });
	}

	parameters p5_15 = { "5-15" };
	for (size_t n = 50; n < 300; n += 50) {
		p5_15.lsrk.push_back({ 5, 15, n, 60 });
	}
	for (size_t n = 300; n < 500; n += 100) {
		p5_15.lsrk.push_back({ 5, 15, n, 60 });
	}
	for (size_t n = 500; n <= 4000; n += 250) {
		p5_15.lsrk.push_back({ 5, 15, n, 60 });
	}

	parameters p5_k1000_part1 = { "5-k-1000" };
	parameters p5_k1000_part2 = { "5-k-1000" };
	parameters p5_k1000_part3 = { "5-k-1000" };
	for (size_t k = 1; k < 20; k += 1) {
		p5_k1000_part1.lsrk.push_back({ 5, k, 1000, 60 });
	}
	for (size_t k = 20; k < 32; k += 2) {
		p5_k1000_part2.lsrk.push_back({ 5, k, 1000, 60 });
	}
	for (size_t k = 32; k < 60; k += 4) {
		p5_k1000_part3.lsrk.push_back({ 5, k, 1000, 60 });
	}

	parameters p5_5_to11000 = { "5-5-to11000" };
	for (size_t n = 1000; n <= 11000; n += 1000) {
		p5_5_to11000.lsrk.push_back({ 5, 5, n, 60 });
	}

	parameters p5_11_to9000 = { "5-11-to9000" };
	for (size_t n = 1000; n <= 9000; n += 1000) {
		p5_11_to9000.lsrk.push_back({ 5, 11, n, 60 });
	}

	parameters p5_2_to6000 = {"5-2-to6000" };
	for (size_t n = 500; n <= 6000; n += 500) {
		p5_2_to6000.lsrk.push_back({ 5, 2, n, 60 });
	}

	parameters p5_12_to6000 = { "5-12-to6000" };
	for (size_t n = 500; n < 6000; n += 500) {
		p5_12_to6000.lsrk.push_back({ 5, 12, n, 60 });
	}
	for (size_t n = 6000; n <= 6000; n += 500) {
		p5_12_to6000.lsrk.push_back({ 5, 12, n, 60 });
	}

	parameters p5_9_to2000 = { "5-9-to2000" };
	for (size_t n = 100; n <= 2000; n += 100) {
		p5_9_to2000.lsrk.push_back({ 5, 9, n, 60 });
	}

	parameters p5_10_to2000 = { "5-10-to2000" };
	for (size_t n = 100; n <= 2000; n += 100) {
		p5_10_to2000.lsrk.push_back({ 5, 10, n, 60 });
	}

	parameters p_variable_5_1000 = { "v-5-1000" };
	for (size_t L = 1; L < 10; L += 1) {
		p_variable_5_1000.lsrk.push_back({ L, 5, 1000, 60 });
	}
	for (size_t L = 10; L < 20; L += 2) {
		p_variable_5_1000.lsrk.push_back({ L, 5, 1000, 60 });
	}
	for (size_t L = 20; L < 100; L += 5) {
		p_variable_5_1000.lsrk.push_back({ L, 5, 1000, 60 });
	}
	for (size_t L = 100; L < 500; L += 25) {
		p_variable_5_1000.lsrk.push_back({ L, 5, 1000, 60 });
	}

	vector<parameters> p5_16_to7000_parts;
	for (size_t n : {500, 1000, 2000, 3000, 4000, 5000, 6000, 7000}) {
		parameters part = { "5-16-to7000" };
		part.lsrk.push_back({ 5, 16, n, 60 });
		part.print_header = (n < 1000);
		p5_16_to7000_parts.push_back(part);
	}

	vector<parameters>  p5_17_to7000_parts;
	for (size_t n : {500, 1000, 2000, 3000, 4000, 5000, 6000, 7000}) {
		parameters part = { "5-17-to7000" };
		part.lsrk.push_back({ 5, 17, n, 60 });
		part.print_header = (n < 1000);
		p5_17_to7000_parts.push_back(part);
	}

	parameters p5_11 = { "5-11" };
	for (size_t n = 500; n <= 9000; n += 500) {
		p5_11.lsrk.push_back({ 5, 11, n, 60 });
	}

	parameters pb_4_x = { "b-4_x" };
	for (size_t n = 200; n <= 3000; n += 200) {
		pb_4_x.lsrk.push_back({ (n-4)/2, 4, n, 60 });
	}

	parameters pb_4 = { "b-4" };
	for (size_t n = 50; n < 500; n += 25) {
		pb_4.lsrk.push_back({ (n - 4) / 2, 4, n, 60 });
	}
	for (size_t n = 500; n <= 1000; n += 50) {
		pb_4.lsrk.push_back({ (n - 4) / 2, 4, n, 60 });
	}
	for (size_t n = 200; n <= 3000; n += 200) {
		pb_4.lsrk.push_back({ (n - 4) / 2, 4, n, 60 });
	}

	parameters p5_4_x = { "5-4_x" };
	for (size_t n = 200; n <= 3000; n += 200) {
		p5_4_x.lsrk.push_back({ 5, 4, n, 60 });
	}

	parameters pb_15 = { "b-15" };
	for (size_t n = 50; n < 300; n += 50) {
		pb_15.lsrk.push_back({ (n - 15) / 2, 15, n, 60 });
	}
	for (size_t n = 300; n < 500; n += 100) {
		pb_15.lsrk.push_back({ (n - 15) / 2, 15, n, 60 });
	}
	for (size_t n = 500; n <= 4000; n += 250) {
		pb_15.lsrk.push_back({ (n - 15) / 2, 15, n, 60 });
	}

	parameters pb_2_to6000 = { "b-2-to6000" };
	for (size_t n = 500; n <= 6000; n += 500) {
		pb_2_to6000.lsrk.push_back({ (n-2)/2, 2, n, 60 });
	}

	parameters pb_12_to6000 = { "b-12-to6000" };
	for (size_t n = 500; n < 6000; n += 500) {
		pb_12_to6000.lsrk.push_back({ (n - 12) / 2, 12, n, 60 });
	}
	for (size_t n = 6000; n <= 6000; n += 500) {
		pb_12_to6000.lsrk.push_back({ (n - 12) / 2, 12, n, 60 });
	}

	parameters pb_9_to2000 = { "b-9-to2000" };
	for (size_t n = 100; n <= 2000; n += 100) {
		pb_9_to2000.lsrk.push_back({ (n-9)/2, 9, n, 60 });
	}

	parameters pb_10_to2000 = { "b-10-to2000" };
	for (size_t n = 100; n <= 2000; n += 100) {
		pb_10_to2000.lsrk.push_back({ (n - 10) / 2, 10, n, 60 });
	}

	parameters p5_15_sparse = { "5-15-sparse" };
	for (size_t n = 500; n <= 4000; n += 500) {
		p5_15_sparse.lsrk.push_back({ 5, 15, n, 60 });
	}

	parameters p5_4_sparse = { "5-4-sparse" };
	for (size_t n = 200; n <= 1000; n += 200) {
		p5_4_sparse.lsrk.push_back({ 5, 4, n, 60 });
	}

	parameters p_variable_5_1000_sparse = { "v-5-1000-sparse" };
	for (size_t L = 1; L < 10; L += 2) {
		p_variable_5_1000_sparse.lsrk.push_back({ L, 5, 1000, 60 });
	}
	for (size_t L = 12; L < 20; L += 4) {
		p_variable_5_1000_sparse.lsrk.push_back({ L, 5, 1000, 60 });
	}
	for (size_t L = 20; L < 100; L += 10) {
		p_variable_5_1000_sparse.lsrk.push_back({ L, 5, 1000, 60 });
	}
	for (size_t L = 100; L < 500; L += 50) {
		p_variable_5_1000_sparse.lsrk.push_back({ L, 5, 1000, 60 });
	}

	parameters p_variable_5_4000 = { "v-5-4000" };
	for (size_t L = 1; L < 10; L += 1) {
		p_variable_5_4000.lsrk.push_back({ L, 5, 4000, 60 });
	}
	for (size_t L = 10; L < 50; L += 10) {
		p_variable_5_4000.lsrk.push_back({ L, 5, 4000, 60 });
	}
	for (size_t L = 50; L < 150; L += 25) {
		p_variable_5_4000.lsrk.push_back({ L, 5, 4000, 60 });
	}
	for (size_t L = 200; L <= 2000; L += 200) {
		p_variable_5_4000.lsrk.push_back({ L, 5, 4000, 60 });
	}

	vector<parameters> p5_4_high;
	for (size_t n = 500; n <= 30000; n = (n == 500 ? 1000 : n + 1000)) {
		parameters part = { "5-4-high" };
		part.lsrk.push_back({ 5, 4, n, 60 });
		part.print_header = (n < 1000);
		p5_4_high.push_back(part);
	}

	vector<parameters> p5_8_high;
	for (size_t n = 500; n <= 30000; n = (n == 500 ? 1000 : n + 1000)) {
		parameters part = { "5_8_high" };
		part.lsrk.push_back({ 5, 8, n, 60 });
		part.print_header = (n < 1000);
		p5_8_high.push_back(part);
	}

	parameters p_5_5_1000_to100 = { "5_5_1000_k" };;
	for (size_t k = 5; k < 10; k += 1) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}
	for (size_t k = 10; k < 20; k += 2) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}
	for (size_t k = 20; k < 35; k += 3) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}
	for (size_t k = 35; k < 100; k += 5) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}
	for (size_t k = 100; k < 200; k += 10) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}
	for (size_t k = 200; k < 400; k += 25) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}
	for (size_t k = 400; k < 1000; k += 50) {
		p_5_5_1000_to100.lsrk.push_back({ 5, 5, 1000, k });
	}

	/*
	 * COMMENT OUT SOME OF THESE LINES IF YOU WANT TO RUN A SUBSET OF THESE EXPERIMENTS.
	 */

	parameter_sets.push_back(p5_4); /* RFG */
	parameter_sets.push_back(p5_7);
	parameter_sets.push_back(p5_8);
	parameter_sets.push_back(p5_15);
	parameter_sets.push_back(p5_k1000_part1);
	parameter_sets.push_back(p5_k1000_part2);
	parameter_sets.push_back(p5_k1000_part3);
	parameter_sets.push_back(p_variable_5_1000);
	parameter_sets.push_back(p_variable_5_4000);

	parameter_sets.push_back(p5_10_to2000); /* RWG Livejournal comparison */
	parameter_sets.push_back(p5_9_to2000);
	parameter_sets.push_back(pb_10_to2000); /* RWG Livejournal comparison (balanced) */
	parameter_sets.push_back(pb_9_to2000);
	
	parameter_sets.push_back(p5_2_to6000); /* RWG BerkStan comparison */
	parameter_sets.push_back(p5_12_to6000);
	parameter_sets.push_back(pb_2_to6000); /* RWG BerkStan comparison (balanced */
	parameter_sets.push_back(pb_12_to6000);
	
	parameter_sets.push_back(p5_5_to11000); /* RWG Epinions comparison */
	parameter_sets.push_back(p5_11_to9000);
	for (auto& part : p5_16_to7000_parts)
		parameter_sets.push_back(part);
	for (auto& part : p5_17_to7000_parts)
		parameter_sets.push_back(part);

	//parameter_sets.push_back(pb_4_x); /* other */
	//parameter_sets.push_back(p5_4_x);
	//parameter_sets.push_back(pb_15);
	//parameter_sets.push_back(p5_4_sparse);
	//parameter_sets.push_back(p5_15_sparse);
	//parameter_sets.push_back(p_variable_5_1000_sparse);
	//for (auto& part : p5_4_high)
	//	parameter_sets.push_back(part);
	//for (auto& part : p5_8_high)
	//	parameter_sets.push_back(part);
	//parameter_sets.push_back(p_5_5_1000_to100);

	for (parameters& parameters : parameter_sets) {
		UndirectedBenchmarker bm;
		configure_bm(bm);
		size_t duplicates_per_param = 5;

		bm.print_level = bm.SUMMARY;
		if(parameters.print_header)
			bm.print_header(std::cout);

		for (LSNK& par : parameters.lsrk) {
			size_t left = par.L;
			size_t middle = par.S;
			size_t right = par.N - par.L - par.S;
			size_t d = par.k;

			std::string graphname = "RFG-" + to_string(left) + "-" + to_string(middle) + "-" + to_string(right) + "-" + to_string(d);
			for (size_t i = 0; i < duplicates_per_param; i++) {
				bm.setGraph(versioned_graph::make_undirected_FG_LSR_graph_adj_list(left, middle, right, d), graphname);
				bm.run();
			}
		}

		cout << std::endl << std::endl;
		bm.print_level = bm.CONDENSED_SUMMARY;
		if (parameters.print_header)
			bm.print_header(cout);
		bm.output(cout);
		cout << std::endl << std::endl;
		bm.print_level = bm.EXTRA_CONDENSED_SUMMARY;
		if (parameters.print_header)
			bm.print_header(cout);
		bm.output(cout);

		if (file_output) {
			ofstream out;
			out.open(output_dir + "RFG/" + parameters.name + ".txt", std::ios_base::app);
			bm.print_level = bm.NORMAL;
			if (parameters.print_header)
				bm.print_header(out);
			bm.output(out);
			out.close();
			out.open(output_dir + "RFG/" + parameters.name + ".csv", std::ios_base::app);
			bm.print_level = bm.EXTRA_CONDENSED_SUMMARY_CSV;
			if (parameters.print_header)
				bm.print_header(out);
			bm.output(out);
		}
	}

}

void main6() {
	ios_base::sync_with_stdio(false);

	string fn1 = input_dir + "RHG/";
	string fn2 = "-5-10+";
	string fn3 = ".txt";

	struct params {
		size_t size;
		size_t i_min = 0;
		size_t i_max = 19;
		bool print_file_header = true; // Specifically in file output
	};
	vector<vector<params>> params_set;
	/*
	 * CHANGE THIS TO RUN A SUBSET OF THE EXPERIMENTS
	 * Originally the data goes up to s=18 but those files
	 * exceed GitHub's recommended maximum file size of 50.00 MB
	 */
	for (size_t s = 10; s <= 17; s++) {
		for (size_t i = 0; i < 20; i++) {
			/* Use this to write to files after a graph has been processed.
			 * Can be useful for the larger instances.
			 */
			params_set.push_back({ {s, i, i, i == 0} }); 
		}
		/* Use this to write to files after all graphs of a certain size have been processed. */ 
		// params_set.push_back({ { s } });
	}

	for (std::vector<params>& param_vec : params_set) {
		UndirectedBenchmarker bm;
		configure_bm(bm);
		bm.print_level = bm.NORMAL;

		for (params& p : param_vec) {
			size_t size_param = p.size;
			bm.print_header(std::cout);

			for (size_t i = p.i_min; i <= p.i_max; i++) {
				std::string graphname = "rhg-" + to_string(size_param) + fn2 + to_string(i);
				std::string filename = fn1 + to_string(size_param) + "/" + to_string(size_param) + fn2 + to_string(i) + fn3;
				bm.read_undirected_graph_from_file(filename, graphname, false);
				bm.run();
			}
			cout << std::endl << std::endl;
			bm.print_level = bm.EXTRA_CONDENSED_SUMMARY;
			bm.print_header(std::cout);
			bm.output(cout);
			cout << std::endl << std::endl;
			if constexpr(file_output) {
				ofstream out;
				out.open(output_dir + "RHG/" + to_string(size_param) + "-5-10.txt", std::ios_base::app);
				bm.print_level = bm.NORMAL;
				if (p.print_file_header)
					bm.print_header(out);
				bm.output(out);
				out.close();
				out.open(output_dir + "RHG/" + to_string(size_param) + "-5-10.csv", std::ios_base::app);
				out.open(output_dir + "RHG/" + to_string(size_param) + "-5-10.csv", std::ios_base::app);
				bm.print_level = bm.EXTRA_CONDENSED_SUMMARY_CSV;
				if (p.print_file_header)
					bm.print_header(out);
				bm.output(out);
			}
		}
	}
}

void main7() {
	using namespace preprocessing;

	bool print_graph_info = false;
	bool benchmarking = true;
	bool save = false;
	bool verbose = false;
	size_t print_edges_in_FG = 0; // 0: do not print, k: print number of edges in FG_k

	typedef std::vector<std::vector<size_t>> adjacency_list;

	string path = input_dir + "RWG/";
	string suffix = ".txt";

	// Most of these did not yield useful results
	string name1 = "soc-Epinions1";
	string name2 = "web-NotreDame"; // No.
	string name3 = "Amazon0601";
	string name4 = "com-orkut.ungraph";
	//string name5 = "gplus_combined"; // weirdly large numbers
	string name6 = "web-Stanford"; // k39-k44 have vertex connectivity 2
	string name7 = "roadNet-TX";
	string name8 = "web-BerkStan"; // k=2 at 77-80, k=12 at 93-101
	string name9 = "web-Google"; // k=1 until 30, then trivial
	string name10 = "com-lj.ungraph"; // Good from 150 to 250.
	string name11 = "wiki-topcats";
	string name12 = "com-youtube.ungraph"; // Good 15
	string name13 = "soc-pokec-relationships";
	string name14 = "com-friendster.ungraph";

	string name114 = "com-friendster.ungraph-k-200";
	
	size_t skip_lines = 0;

	string presuffix = "";

	struct params_t {
		string name;
		vector<size_t> k_values;
	};

	/*
	 * COMMENT OUT SOME OF THESE LINES IF YOU WANT TO RUN A SUBSET OF THESE EXPERIMENTS.
	 * Berkstan and livejournal exceed GitHub's recommended maximum file size of 50.00 MB
	 */
	std::vector<params_t> params = {
		//{ name8, {80, 93} }, // Berkstan
		{ name1, {9, 12, 17, 19} }, // Epinions
		//{ name10, {160, 200, 231, 250} }, // livejournal
	};

	for(params_t& par : params) {
		adjacency_list G;
		std::cout << "Reading graph at " << path + par.name + presuffix + suffix << std::endl;
		{
			ifstream in;
			in.open(path + par.name + presuffix + suffix);
			G = read_adjlist_from_edgelist_undirected(in);
		}
		if (G.size() == 0) {
			std::cout << "Skipping 4 lines." << std::endl;
			skip_lines = 4;
			ifstream in;
			in.open(path + par.name + presuffix + suffix);
			G = read_adjlist_from_edgelist_undirected(in, skip_lines);
		}

		if (verbose) cout << "About to remove duplicate edges." << endl;
		preprocessing::inplace_remove_duplicate_edges(G);
		if (verbose) cout << "Removed duplicate edges." << endl;
		size_t n = G.size();
		size_t m = 0;
		for (vector<size_t>& vec : G)
			m += vec.size();

		cout << par.name + presuffix << " has " << n << " vertices and " << m / 2 << " * 2 edges." << endl;

		UndirectedBenchmarker bm;
		configure_bm(bm);
		bm.N = 1;

		bm.print_level = bm.QUIET;
		if (benchmarking && !print_graph_info) {
			bm.print_level = bm.NORMAL;
			bm.print_header(std::cout);
		}
		
		for (size_t k : par.k_values) {
			// k-core stuff
			if (true) {
				std::string graphname = par.name + "-k-" + to_string(k);

				adjacency_list G2 = LCC_of_kcore(G, k);
				
				/*
				auto& G2 = G;
				if (verbose) cout << "About to compute in place k-core." << endl;
				in_place_k_core_undirected(G2, k);
				if (verbose) cout << "Computed in place k-core." << endl;
				if (verbose) cout << "About to compute in place LCC." << endl;
				in_place_LWCC(G2);
				if (verbose) cout << "Computed LCC of k-core." << endl;
				*/

				// save
				if (save) {
					ofstream out;
					out.open(path + graphname + suffix);
					write_adjlist_to_edgelist_undirected(out, G2);
				}

				size_t d_min = SIZE_MAX;
				for (vector<size_t>& vec : G2) {
					if (d_min > vec.size())
						d_min = vec.size();
				}

				size_t n1 = G2.size();
				size_t m1 = 0;
				for (vector<size_t>& vec : G2)
					m1 += vec.size();
				// Graph info.
				if (print_graph_info)
					cout << "LLC of " << graphname << " has " << n1 << " vertices and " << m1 / 2 << " * 2 edges with minimum degree " << d_min << endl;
				// Stop when k-cores become trivial.
				if (true)
					if (n1 == 0 || m1 == 0) break;
				// Benchmarker.
				if (benchmarking) {
					if (n1 == 0 || m1 == 0) continue;

					bm.setGraph(G2, graphname);
					if (benchmarking && print_graph_info)
						bm.print_header(std::cout);
					bm.run();
				}
				if (print_edges_in_FG) {
					sparse::labelling lab = sparse::nagamochi_ibraki_labelling(G2);
					adj_list FG = sparse::nagamochi_ibraki_adj_list(lab, print_edges_in_FG);
					size_t m = 0; 
					for (auto& vec : FG)
						m += vec.size();
					cout << "FG_" << print_edges_in_FG << " has " << m << " edges." << endl;
				}

				// Skip to where the graph changes.
				if (true) {
					k = d_min;
				}
			}
		}
		cout << endl;
		if (benchmarking) {
			bm.print_level = bm.SUMMARY;
			bm.print_header(std::cout);
			bm.output(cout);
		}
		cout << endl;
		if (benchmarking) {
			bm.print_level = bm.CONDENSED_SUMMARY;
			bm.print_header(std::cout);
			bm.output(cout);
		}
		if (file_output) {
			ofstream out;
			out.open(output_dir + "RWG/" + par.name + ".txt", std::ios_base::app);
			bm.print_level = bm.NORMAL;
			bm.print_header(out);
			bm.output(out);
			out.close();
			out.open(output_dir + "RWG/" + par.name + ".csv", std::ios_base::app);
			bm.print_level = bm.EXTRA_CONDENSED_SUMMARY_CSV;
			bm.print_header(out);
			bm.output(out);
			out.open(output_dir + "RWG/rwg.txt", std::ios_base::app);
			bm.print_level = bm.NORMAL;
			bm.print_header(out);
			bm.output(out);
			out.close();
			out.open(output_dir + "RWG/rwg.csv", std::ios_base::app);
			bm.print_level = bm.EXTRA_CONDENSED_SUMMARY_CSV;
			bm.print_header(out);
			bm.output(out);
		}

	}
}



void main10() {
	adj_list G;
	enum struct e {
		rhg = 0,
		lsr = 1,
		er = 2,
	} use = e::er;
	size_t sparsify = 10;
	bool benchmark = true;

	if (use == e::rhg) {
		string fn1 = "C:/Users/Public/Documents/RHG/";
		string fn2 = "-5-10+";
		string fn3 = ".txt";

		size_t size_param = 17;
		size_t i = 0;

		std::string filename = fn1 + to_string(size_param) + "/" + to_string(size_param) + fn2 + to_string(i) + fn3;

		ifstream in;
		in.open(filename);
		G = read_adjlist_from_edgelist_undirected(in);
	}
	else if (use == e::lsr){
		G = versioned_graph::make_undirected_FG_LSR_graph_adj_list(5, 5, 990, 10);
	}
	else if (use == e::er) {
		size_t n = 10000;
		G.resize(n);

		size_t d = 32;
		double p = d / (double)n;
		auto& r = std::bernoulli_distribution(p);

		size_t L = 5;
		size_t S = 5;

		for (size_t v = 0; v < n; v++) {
			for (size_t u = v+1; u < n; u++) {
				if (r(random_engine)) {
					G[v].push_back(u);
					G[u].push_back(v);
				}
			}
		}
	}

	std::cout << "Generated graph" << std::endl;

	write_degree_distribution(std::cout, G);

	if (sparsify) {
		auto labelling = sparse::nagamochi_ibraki_labelling(G);
		std::cout << std::endl << std::endl;
		write_degree_distribution(std::cout, sparse::nagamochi_ibraki_adj_list(labelling, sparsify));
	}

	if (benchmark) {
		UndirectedBenchmarker bm;
		configure_bm(bm);
		bm.setGraph(G, "G");
		bm.print_level = bm.QUIET;
		bm.run();
		bm.print_level = bm.EXTRA_CONDENSED_SUMMARY;
		bm.print_header(std::cout);
		bm.output(std::cout);
		std::cout << std::endl << std::endl;
	}
}