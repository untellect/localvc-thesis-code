#include "pch.h"

#include "Benchmarker.h"
#include "Benchmark.h"
#include "GraphIO.h"

#include <fstream>

std::string pad_string(std::string s, size_t target = 10) {
	size_t n = s.size();
	if (n < target)
		return s.append(target - n, ' ');
	else
		return s;
}


std::string pad_size_t(size_t x, size_t target = 10) {
	return pad_string(to_string(x), target);
}

void UndirectedBenchmarker::read_undirected_graph_from_file(std::string filename, std::string graphname, bool printinfo) {
	ifstream in;
	in.tie(NULL);
	in.open(filename);

	auto t_0 = high_resolution_clock::now();
	adj_list G = read_adjlist_from_edgelist_undirected(in);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);
	if (printinfo) {
		cout << "It took " << dt.count() << " ms to read the file." << endl;
		graph g(adjacency_list);
		cout << "G has " << g.size_vertices() << " vertices and " << g.size_edges() << " edges." << endl;
	}
	setGraph(G, graphname);
}

void UndirectedBenchmarker::write_undirected_graph_to_file(std::string filename, bool printinfo) {
	ofstream out;
	out.tie(NULL);
	out.open(filename);

	auto t_0 = high_resolution_clock::now();
	write_adjlist_to_edgelist_undirected(out, adjacency_list);
	auto t_1 = high_resolution_clock::now();
	auto dt = duration_cast<std::chrono::milliseconds>(t_1 - t_0);

	throw 0;
}

void UndirectedBenchmarker::setGraph(adj_list& G, std::string name) {
	adjacency_list = G;
	graphid = name;
	if(graphid.size() < 20)
		graphid.append(20 - graphid.size(), ' ');
	updateGraphMeta();

	results.push_back({});
	results.back().graphid = graphid;
	results.back().n = n;
	results.back().m = m;
	results.back().d_min = d_min;
}

void UndirectedBenchmarker::updateGraphMeta() {
	n = adjacency_list.size();
	m = 0;
	d_min = SIZE_MAX;
	for (auto& vec : adjacency_list) {
		m += vec.size();
		d_min = min(d_min, vec.size());
	}
		
}

void UndirectedBenchmarker::run_local_based(size_t ver) {
	size_t b = localvc_cfg[ver].boost;

	results.back().local[ver].resize(N);
	auto& res = results.back().local[ver];
	results.back().N = N;

	for (size_t i = 0; i < N; i++) {
		auto t_0 = high_resolution_clock::now();
		std::unordered_set<vertex> min_cut = find_vertex_connectivity_undirected(localvc_cfg[ver].vc, localvc_cfg[ver].localec, localvc_cfg[ver].unbalancedvc, localvc_cfg[ver].balancedvc, adjacency_list, localvc_cfg[ver].a_coeff, b);
		auto t_1 = high_resolution_clock::now();

		res[i].t = t_1 - t_0;
		res[i].k = min_cut.size();
		if (results.back().k_min > res[i].k)
			results.back().k_min = res[i].k;
		res[i].boost = b;
		res[i].separator = min_cut;
		//res[i].x = 0; // TODO
		//res[i].volume = 0; // TODO;

		if (print_level == NORMAL || print_level == EXTRA) {
			std::cout
				<< pad_size_t(duration_cast<std::chrono::microseconds>(res[i].t).count())
				<< "\t" << res[i].k
				<< "\t" << d_min
				<< "\t" << n
				<< "\t" << m
				<< "\t" << pad_string(localvc_cfg[ver].name)
				<< "\t" << graphid << std::endl;
		}
	}

	if (print_level == SUMMARY) {
		chrono::steady_clock::duration t_avg = 0s;
		size_t t_min = SIZE_MAX;
		size_t t_max = 0;
		size_t k_min = SIZE_MAX;
		size_t k_max = 0;
		for(auto& r : res) {
			size_t t = duration_cast<microseconds>(r.t).count();
			size_t k = r.k;
			t_avg += r.t;
			t_min = min(t_min, t);
			t_max = max(t_max, t);
			k_min = min(k_min, k);
			k_max = max(k_max, k);
		}
		size_t successes = 0;
		for (size_t i = 0; i < N; i++) {
			if (res[i].k == k_min)
				successes++;
		}
		double p_success = floor(1000 * (double)successes / (double)N) / 10;
		t_avg /= N;
		//std::cout << "t_avg\tt_min\tt_max\tk_min\tk_max\talg\tgraph" << std::endl;
		std::cout
			<< pad_size_t(duration_cast<std::chrono::microseconds>(t_avg).count())
			<< "\t" << pad_size_t(t_min)
			<< "\t" << pad_size_t(t_max)
			<< "\t" << k_min
			<< "\t" << k_max
			<< "\t" << d_min
			<< "\t" << p_success
			<< "\t" << N
			<< "\t" << n
			<< "\t" << m
			<< "\t" << pad_string(localvc_cfg[ver].name)
			<< "\t" << graphid << std::endl;
	}
}
void UndirectedBenchmarker::run_local2_based(size_t ver) {
	results.back().local2[ver].resize(N);
	auto& res = results.back().local2[ver];
	results.back().N = N;

	size_t b = 1;

	for (size_t i = 0; i < N; i++) {
		auto t_0 = high_resolution_clock::now();
		std::unordered_set<vertex> min_cut = localvc2_cfg[ver].find_vc(localvc2_cfg[ver].vc, localvc2_cfg[ver].localec, localvc2_cfg[ver].unbalancedvc, localvc2_cfg[ver].balancedvc, adjacency_list);
		auto t_1 = high_resolution_clock::now();

		res[i].t = t_1 - t_0;
		res[i].k = min_cut.size();
		if (results.back().k_min > res[i].k)
			results.back().k_min = res[i].k;
		res[i].boost = b;
		res[i].separator = min_cut;
		//res2[i].x = 0; // TODO
		//res2[i].volume = 0; // TODO;

		if (print_level == NORMAL || print_level == EXTRA) {
			std::cout
				<< pad_size_t(duration_cast<std::chrono::microseconds>(res[i].t).count())
				<< "\t" << res[i].k
				<< "\t" << d_min
				<< "\t" << n
				<< "\t" << m
				<< "\t" << pad_string(localvc2_cfg[ver].name)
				<< "\t" << graphid << std::endl;
		}
	}

	if (print_level == SUMMARY) {
		chrono::steady_clock::duration t_avg = 0s;
		size_t t_min = SIZE_MAX;
		size_t t_max = 0;
		size_t k_min = SIZE_MAX;
		size_t k_max = 0;
		for (size_t i = 0; i < N; i++) {
			size_t t = duration_cast<microseconds>(res[i].t).count();
			size_t k = res[i].k;
			t_avg += res[i].t;
			t_min = min(t_min, t);
			t_max = max(t_max, t);
			k_min = min(k_min, k);
			k_max = max(k_max, k);
		}
		size_t successes = 0;
		for (size_t i = 0; i < N; i++) {
			if (res[i].k == k_min)
				successes++;
		}
		double p_success = floor(1000 * (double)successes / (double)N) / 10;
		t_avg /= N;
		//std::cout << "t_avg\tt_min\tt_max\tk_min\tk_max\talg\tgraph" << std::endl;
		std::cout
			<< pad_size_t(duration_cast<std::chrono::microseconds>(t_avg).count())
			<< "\t" << pad_size_t(t_min)
			<< "\t" << pad_size_t(t_max)
			<< "\t" << k_min
			<< "\t" << k_max
			<< "\t" << d_min
			<< "\t" << p_success
			<< "\t" << N
			<< "\t" << n
			<< "\t" << m
			<< "\t" << pad_string(localvc2_cfg[ver].name)
			<< "\t" << graphid << std::endl;
	}
}

void UndirectedBenchmarker::run_henzinger(size_t ver) {
	results.back().henz[ver].resize(N);
	auto& res = results.back().henz[ver];

	results.back().N = N;

	size_t x;

	for (size_t i = 0; i < N; i++) {
		auto& r = res[i];

		vector<henzinger::henzinger_subresult> subresults;

		auto t_0 = high_resolution_clock::now();
		std::unordered_set<vertex> min_cut = henzinger_cfg[ver].vc(adjacency_list, x, subresults);
		auto t_1 = high_resolution_clock::now();

		r.t = t_1 - t_0;
		r.k = min_cut.size();
		if (results.back().k_min > r.k)
			results.back().k_min = r.k;
		r.separator = min_cut;
		r.subresults = subresults;
		//r.x = 0; // TODO
		//r.volume = 0; // TODO;

		if (print_level == NORMAL || print_level == EXTRA) {
			std::cout
				<< pad_size_t(duration_cast<microseconds>(r.t).count())
				<< "\t" << r.k
				<< "\t" << d_min
				<< "\t" << n
				<< "\t" << m
				<< "\t" << pad_string(henzinger_cfg[ver].name)
				<< "\t" << graphid;
			if (print_level == NORMAL)
				std::cout << std::endl;
			if (print_level == EXTRA) {
				std::cout << "\t";
				for (auto& subres : r.subresults)
					std::cout << duration_cast<microseconds>(subres.t).count() << " ";
				std::cout << std::endl;
			}
		}
	}

	if (print_level == SUMMARY) {
		chrono::steady_clock::duration t_avg = 0s;
		size_t t_min = SIZE_MAX;
		size_t t_max = 0;
		size_t k_min = SIZE_MAX;
		size_t k_max = 0;
		for (size_t i = 0; i < N; i++) {
			auto& r = res[i];


			size_t t = duration_cast<microseconds>(r.t).count();
			size_t k = r.k;
			t_avg += r.t;
			t_min = min(t_min, t);
			t_max = max(t_max, t);
			k_min = min(k_min, k);
			k_max = max(k_max, k);
		}
		size_t successes = 0;
		for (size_t i = 0; i < N; i++) {
			if (res[i].k == results.back().k_min)
				successes++;
		}
		double p_success = floor(1000 * (double)successes / (double)N) / 10;
		t_avg /= N;
		//std::cout << "t_avg\tt_min\tt_max\tk_min\tk_max\talg\tgraph" << std::endl;
		std::cout
			<< pad_size_t(duration_cast<std::chrono::microseconds>(t_avg).count())
			<< "\t" << pad_size_t(t_min)
			<< "\t" << pad_size_t(t_max)
			<< "\t" << k_min
			<< "\t" << k_max
			<< "\t" << d_min
			<< "\t" << p_success
			<< "\t" << N
			<< "\t" << n
			<< "\t" << m
			<< "\t" << pad_string(henzinger_cfg[ver].name)
			<< "\t" << graphid << std::endl;
	}
}

void UndirectedBenchmarker::print_header(std::ostream& out) {
	if (print_level == NORMAL)
		out << "t(us)\t\tk\td_min\tn\tm\talg\t\tgraph" << std::endl;
	if (print_level == EXTRA)
		out << "t(us)\t\tk\td_min\tn\tm\talg\t\tgraph\t\t\tt_sub(us)" << std::endl;
	if(print_level == SUMMARY)
		out << "t_avg(us)\tt_min(us)\tt_max(us)\tk_min\tk_max\td_min\t%\tN\tn\tm\talg\t\tgraph" << std::endl;
	if (print_level == CONDENSED_SUMMARY || print_level == EXTRA_CONDENSED_SUMMARY) {
		out << "N\tn\tm\td_min\tk_min\tk_fg\tm_fg";
		for (size_t i = 0; i < LOCAL1_VERSIONS; i++) {
			if (localvc_cfg[i].on) {
				out << "\tt(" << localvc_cfg[i].name << ")(us)\t%(" << localvc_cfg[i].name << ")";
			}
		}
		for (size_t i = 0; i < LOCAL2_VERSIONS; i++) {
			if (localvc2_cfg[i].on) {
				out << "\tt(" << localvc2_cfg[i].name << ")(us)\t%(" << localvc2_cfg[i].name << ")";
			}
		}
		for (size_t i = 0; i < HENZ_VERSIONS; i++) {
			if (henzinger_cfg[i].on)
				out << "\tt(" << henzinger_cfg[i].name << ")(us)\t%(" << henzinger_cfg[i].name << ")";
		}
		out << "\tgraph" << std::endl;
	}
	if (print_level == EXTRA_CONDENSED_SUMMARY_CSV) {
		out << "N,n,m,dmin,k,kfg,mfg";
		for (size_t i = 0; i < LOCAL1_VERSIONS; i++) {
			if (localvc_cfg[i].on) {
				out << "," << localvc_cfg[i].name << ",P(" << localvc_cfg[i].name << ")";
			}
		}

		for (size_t i = 0; i < LOCAL2_VERSIONS; i++) {
			if (localvc2_cfg[i].on) {
				out << "," << localvc2_cfg[i].name << ",P(" << localvc2_cfg[i].name << ")";
			}
		}
		for (size_t i = 0; i < HENZ_VERSIONS; i++) {
			if (henzinger_cfg[i].on)
				out << "," << henzinger_cfg[i].name << ",P(" << henzinger_cfg[i].name << ")";
		}
		out << ",graph" << std::endl;
	}
}



void UndirectedBenchmarker::output(std::ostream& out) {
	if (print_level == QUIET) return;

	if (print_level == CONDENSED_SUMMARY || print_level == EXTRA_CONDENSED_SUMMARY || print_level == EXTRA_CONDENSED_SUMMARY_CSV) {
		size_t graphs = 0;
		size_t res_size = results.size();

		chrono::steady_clock::duration t_local[LOCAL1_VERSIONS];
		chrono::steady_clock::duration t_local2[LOCAL2_VERSIONS];
		chrono::steady_clock::duration t_h[HENZ_VERSIONS];
		for (size_t i = 0; i < LOCAL1_VERSIONS; i++) t_local[i] = 0s;
		for (size_t i = 0; i < LOCAL2_VERSIONS; i++) t_local2[i] = 0s;
		for (size_t i = 0; i < HENZ_VERSIONS; i++) t_h[i] = 0s;
		size_t success_local[LOCAL1_VERSIONS];
		size_t success_local2[LOCAL2_VERSIONS];
		size_t success_h[HENZ_VERSIONS];
		for (size_t i = 0; i < LOCAL1_VERSIONS; i++) success_local[i] = 0;
		for (size_t i = 0; i < LOCAL2_VERSIONS; i++) success_local2[i] = 0;
		for (size_t i = 0; i < HENZ_VERSIONS; i++) success_h[i] = 0;
		std::string id = "";

		for(size_t i = 0; i <= res_size; i++) {
			bool print_extra_condensed_summary = i == res_size || results[i].graphid.compare(id); // last iteration or changed graph

			if (i != 0 && (print_level == CONDENSED_SUMMARY || print_extra_condensed_summary)) {
				size_t N = results[i - 1].N;
				size_t n = results[i - 1].n;
				size_t m = results[i - 1].m;
				size_t d_min = results[i - 1].d_min;
				size_t k_min = results[i - 1].k_min;
				size_t fg_k = results[i - 1].fg_k;
				size_t fg_m = results[i - 1].fg_m;

				char delimiter = (print_level == EXTRA_CONDENSED_SUMMARY_CSV ? ',' : '\t');
				size_t padding = (print_level == EXTRA_CONDENSED_SUMMARY_CSV ? 0 : 15);

				if (graphs != 1)
					out << graphs << "*";
				out << N << delimiter
					<< n << delimiter
					<< m << delimiter
					<< d_min << delimiter
					<< k_min << delimiter
					<< fg_k << delimiter
					<< fg_m;

				for (size_t ver = 0; ver < LOCAL1_VERSIONS; ver++) {
					if (!localvc_cfg[ver].on) continue;

					if (print_level == EXTRA_CONDENSED_SUMMARY_CSV)
						out << delimiter << (double)duration_cast<microseconds>(t_local[ver] / (N * graphs)).count() / 1000000;
					else
						out << delimiter << pad_size_t(duration_cast<microseconds>(t_local[ver] / (N * graphs)).count(), padding);
					out << delimiter << pad_size_t(floor(1000 * (double)success_local[ver] / ((double)N * graphs)) / 10, padding);

					t_local[ver] = 0s;
					success_local[ver] = 0;
				}

				for (size_t ver = 0; ver < LOCAL2_VERSIONS; ver++) {
					if (!localvc2_cfg[ver].on) continue;

					if (print_level == EXTRA_CONDENSED_SUMMARY_CSV)
						out << delimiter << (double)duration_cast<microseconds>(t_local2[ver] / (N * graphs)).count() / 1000000;
					else
						out << delimiter << pad_size_t(duration_cast<microseconds>(t_local2[ver] / (N * graphs)).count(), padding);
					out << delimiter << pad_size_t(floor(1000 * (double)success_local2[ver] / ((double)N * graphs)) / 10, padding);

					t_local2[ver] = 0s;
					success_local2[ver] = 0;
				}

				for (size_t h_ver = 0; h_ver < HENZ_VERSIONS; h_ver++) {
					if (!henzinger_cfg[h_ver].on) continue;

					if (print_level == EXTRA_CONDENSED_SUMMARY_CSV)
						out << delimiter << (double)duration_cast<microseconds>(t_h[h_ver] / (N * graphs)).count() / 1000000;
					else
						out << delimiter << pad_size_t(duration_cast<microseconds>(t_h[h_ver] / (N * graphs)).count(), padding);
					out << delimiter << pad_size_t(floor(1000 * (double)success_h[h_ver] / ((double)N * graphs)) / 10, padding);

					t_h[h_ver] = 0s;
					success_h[h_ver] = 0;
				}

				out << delimiter << id << std::endl;

				graphs = 0;
			}
			if (i == res_size) return;
			graphs += 1;
			id = results[i].graphid;
			// iteration res_size is only there to print the last iteration. After that we're done with output.

			

			for (size_t ver = 0; ver < LOCAL1_VERSIONS; ver++) {
				if (!localvc_cfg[ver].on) continue;
				
				for (auto& r : results[i].local[ver]) {
					t_local[ver] += r.t;
					if (r.k == results[i].k_min)
						success_local[ver]++;
				}
			}

			for (size_t ver = 0; ver < LOCAL2_VERSIONS; ver++) {
				if (!localvc2_cfg[ver].on) continue;

				for (auto& r : results[i].local2[ver]) {
					t_local2[ver] += r.t;
					if (r.k == results[i].k_min)
						success_local2[ver]++;
				}
			}

			for (size_t h_ver = 0; h_ver < HENZ_VERSIONS; h_ver++) {
				if (!henzinger_cfg[h_ver].on) continue;

				for (auto& r : results[i].henz[h_ver]) {
					t_h[h_ver] += r.t;
					if (r.k == results[i].k_min)
						success_h[h_ver]++;
				}
			}
		}
	}

	for (size_t ver = 0; ver < LOCAL1_VERSIONS; ver++) {
		if (!localvc_cfg[ver].on) continue;

		for (auto& res : results) {
			if (print_level == NORMAL || print_level == EXTRA) {
				for (auto& r : res.local[ver]) {
					out << pad_size_t(duration_cast<std::chrono::microseconds>(r.t).count())
						<< "\t" << r.k
						<< "\t" << res.d_min
						<< "\t" << res.n
						<< "\t" << res.m
						<< "\t" << pad_string(localvc_cfg[ver].name)
						<< "\t" << res.graphid << std::endl;
				}
			}
			if (print_level == SUMMARY) {
				chrono::steady_clock::duration t_avg = 0s;
				size_t t_min = SIZE_MAX;
				size_t t_max = 0;
				size_t k_min = SIZE_MAX;
				size_t k_max = 0;
				for (auto& r : res.local[ver]) {
					size_t t = duration_cast<microseconds>(r.t).count();
					size_t k = r.k;
					t_avg += r.t;
					t_min = min(t_min, t);
					t_max = max(t_max, t);
					k_min = min(k_min, k);
					k_max = max(k_max, k);
				}
				size_t successes = 0;
				for (auto& r : res.local[ver]) {
					if (r.k == k_min)
						successes++;
				}
				double p_success = floor(1000 * (double)successes / (double)N) / 10;
				t_avg /= N;
				out << pad_size_t(duration_cast<microseconds>(t_avg).count())
					<< "\t" << pad_size_t(t_min)
					<< "\t" << pad_size_t(t_max)
					<< "\t" << k_min
					<< "\t" << k_max
					<< "\t" << res.d_min
					<< "\t" << p_success
					<< "\t" << res.N
					<< "\t" << res.n
					<< "\t" << res.m
					<< "\t" << pad_string(localvc_cfg[ver].name)
					<< "\t" << res.graphid << std::endl;
			}
		}
	}
	for (size_t ver = 0; ver < LOCAL2_VERSIONS; ver++) {
		if (!localvc2_cfg[ver].on) continue;

		for (auto& res : results) {
			if (print_level == NORMAL || print_level == EXTRA) {
				for (auto& r : res.local2[ver]) {
					out << pad_size_t(duration_cast<std::chrono::microseconds>(r.t).count())
						<< "\t" << r.k
						<< "\t" << res.d_min
						<< "\t" << res.n
						<< "\t" << res.m
						<< "\t" << pad_string(localvc2_cfg[ver].name)
						<< "\t" << res.graphid << std::endl;
				}
			}
			if (print_level == SUMMARY) {
				chrono::steady_clock::duration t_avg = 0s;
				size_t t_min = SIZE_MAX;
				size_t t_max = 0;
				size_t k_min = SIZE_MAX;
				size_t k_max = 0;
				for (auto& r : res.local2[ver]) {
					size_t t = duration_cast<microseconds>(r.t).count();
					size_t k = r.k;
					t_avg += r.t;
					t_min = min(t_min, t);
					t_max = max(t_max, t);
					k_min = min(k_min, k);
					k_max = max(k_max, k);
				}
				size_t successes = 0;
				for (auto& r : res.local2[ver]) {
					if (r.k == k_min)
						successes++;
				}
				double p_success = floor(1000 * (double)successes / (double)N) / 10;
				t_avg /= N;
				out << pad_size_t(duration_cast<microseconds>(t_avg).count())
					<< "\t" << pad_size_t(t_min)
					<< "\t" << pad_size_t(t_max)
					<< "\t" << k_min
					<< "\t" << k_max
					<< "\t" << res.d_min
					<< "\t" << p_success
					<< "\t" << res.N
					<< "\t" << res.n
					<< "\t" << res.m
					<< "\t" << pad_string(localvc2_cfg[ver].name)
					<< "\t" << res.graphid << std::endl;
			}
		}
	}
	for (size_t h_ver = 0; h_ver < HENZ_VERSIONS; h_ver++) {
		if (!henzinger_cfg[h_ver].on) continue;

		for (auto& res : results) {
			auto& h_res = res.henz[h_ver];
			if (print_level == NORMAL) {
				for (auto& r : h_res) {
					out << pad_size_t(duration_cast<microseconds>(r.t).count())
						<< "\t" << r.k
						<< "\t" << res.d_min
						<< "\t" << res.n
						<< "\t" << res.m
						<< "\t" << pad_string(henzinger_cfg[h_ver].name)
						<< "\t" << res.graphid << std::endl;
				}
			}
			if (print_level == EXTRA) {
				for (auto& r : h_res) {
					out << pad_size_t(duration_cast<microseconds>(r.t).count())
						<< "\t" << r.k
						<< "\t" << res.d_min
						<< "\t" << res.N
						<< "\t" << res.n
						<< "\t" << res.m
						<< "\t" << pad_string(henzinger_cfg[h_ver].name)
						<< "\t" << res.graphid
						<< "\t";
					for (auto& subres : r.subresults)
						out << pad_size_t(duration_cast<microseconds>(subres.t).count()) << " ";
					out << std::endl;
				}
			}
			if (print_level == SUMMARY) {
				chrono::steady_clock::duration t_avg = 0s;
				size_t t_min = SIZE_MAX;
				size_t t_max = 0;
				size_t k_min = SIZE_MAX;
				size_t k_max = 0;
				for (auto& r : h_res) {
					size_t t = duration_cast<microseconds>(r.t).count();
					size_t k = r.k;
					t_avg += r.t;
					t_min = min(t_min, t);
					t_max = max(t_max, t);
					k_min = min(k_min, k);
					k_max = max(k_max, k);
				}
				size_t successes = 0;
				for (auto& r : h_res) {
					if (r.k == res.k_min)
						successes++;
				}
				double p_success = floor(1000 * (double)successes / (double)N) / 10;
				t_avg /= N;
				out << pad_size_t(duration_cast<microseconds>(t_avg).count())
					<< "\t" << pad_size_t(t_min)
					<< "\t" << pad_size_t(t_max)
					<< "\t" << k_min
					<< "\t" << k_max
					<< "\t" << res.d_min
					<< "\t" << p_success
					<< "\t" << res.N
					<< "\t" << res.n
					<< "\t" << res.m
					<< "\t" << pad_string(henzinger_cfg[h_ver].name)
					<< "\t" << res.graphid << std::endl;
			}
		}
	}
}

void UndirectedBenchmarker::clear_results() {
	results.clear();
}

void UndirectedBenchmarker::run() {
	for (size_t ver = 0; ver < LOCAL1_VERSIONS; ver++) {
		if (localvc_cfg[ver].on) {
			run_local_based(ver);
		}
	}
	for (size_t ver = 0; ver < LOCAL2_VERSIONS; ver++) {
		if (localvc2_cfg[ver].on) {
			run_local2_based(ver);
		}
	}
	for (size_t ver = 0; ver < HENZ_VERSIONS; ver++) {
		if (henzinger_cfg[ver].on)
			run_henzinger(ver);
	}

	size_t fg_k = 1;
	size_t k_min = results.back().k_min;
	while (fg_k <= k_min)
		fg_k *= 2;

	if (fg_k != results.back().fg_k) {
		results.back().fg_k = fg_k;
		auto FG = sparse::nagamochi_ibraki_adj_list(sparse::nagamochi_ibraki_labelling(adjacency_list), fg_k);
		results.back().fg_m = 0;
		for (auto& vec : FG) {
			results.back().fg_m += vec.size();
		}
	}
}