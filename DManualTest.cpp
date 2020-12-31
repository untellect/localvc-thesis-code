#include "pch.h"

#include "Graphgen.h"

#include "Mains.h"


inline double get_logP(size_t nu, size_t s, size_t k, size_t m) {
	if (s * k > nu) {
		double localec_accuracy = pow(1.0 - nu / ((double)s * k), k - 1);
		return log(1.0 - nu / (2.0 * m) * localec_accuracy);
	}
	else {
		return 0; // Success is impossible at this call volume nu and cut size s.
	}
}

inline std::vector<double> get_logP_vec(const std::vector<size_t>& L, const std::vector<size_t>& samplesize, size_t k, size_t m, size_t alpha) {
	const size_t N = L.size();
	std::vector<double> logP(N);

	for (size_t i = 0; i < N; i++) {
		size_t nu = L[i];
		logP[i] = 0;
		for (size_t j = 0; j < N; j++) {
			size_t s = alpha * L[j];
			logP[i] += samplesize[j] * get_logP(nu, s, k, m);
		}
	}

	return logP;
}

std::tuple<std::vector<size_t>, std::vector<size_t>> low_vol_first(double target_logp, size_t m, size_t k, size_t a) {
	std::vector<double> logP;
	std::vector<size_t> samplesize;
	std::vector<size_t> L;
	for (size_t s = 2; s <= a; s *= 2) {
		L.push_back(s);
		samplesize.push_back(0);
		logP.push_back(0.0);
	}

	for (size_t i = 0; i < L.size(); i++) {
		const size_t& s = L[i];
		double logp_per_sample = log(1.0 - s / (2.0 * m) * pow(1.0 - 1.0 / k, k - 1));
		double delta_s = (target_logp - logP[i]) / logp_per_sample;
		samplesize[i] = (size_t)ceil(delta_s);
		for (size_t j = 0; j < L.size(); ++j) {
			size_t nu = L[j];
			double localec_accuracy = (s * k > nu) ? (pow(1.0 - nu / ((double)s * k), k - 1)) : 0;
			double temp = samplesize[i] * log(1.0 - nu / (2.0 * m) * localec_accuracy);
			logP[j] += temp;
		}
	}

	return { samplesize, L };
}

std::tuple<std::vector<size_t>, std::vector<size_t>> high_vol_first(double target_logp, size_t m, size_t k, size_t a) {
	std::vector<double> logP;
	std::vector<size_t> samplesize;
	std::vector<size_t> L;
	for (size_t s = 2; s <= a; s *= 2) {
		L.push_back(s);
		samplesize.push_back(0);
		logP.push_back(0.0);
	}
	for (size_t i = L.size(); i > 0;) {
		i -= 1;
		const size_t& s = L[i];
		double logp_per_sample = log(1.0 - s / (2.0 * m) * pow(1.0 - 1.0 / k, k - 1));
		double delta_s = (target_logp - logP[i]) / logp_per_sample;
		samplesize[i] = (size_t)ceil(delta_s);
		for (size_t j = 0; j < L.size(); ++j) {
			size_t nu = L[j];
			double localec_accuracy = (s * k > nu) ? (pow(1.0 - nu / ((double)s * k), k - 1)) : 0;
			logP[j] += samplesize[i] * log(1.0 - nu / (2.0 * m) * localec_accuracy);
		}
	}

	return { samplesize, L };
}

std::tuple<std::vector<size_t>, std::vector<size_t>> high_vol_first_iterative(double target_logp, size_t m, size_t k, size_t a) {
	std::vector<double> logP;
	std::vector<size_t> samplesize;
	std::vector<size_t> L;
	for (size_t s = 2; s <= a; s *= 2) {
		L.push_back(s);
		samplesize.push_back(0);
		logP.push_back(0.0);
	}
	size_t iterations = 3;
	for (size_t j = 1; j <= iterations; ++j) {
		double part_target_logp = (target_logp * j) / iterations;
		for (size_t i = L.size(); i > 0;) {
			i -= 1;
			const size_t& s = L[i];
			double logp_per_sample = log(1.0 - s / (2.0 * m) * pow(1.0 - 1.0 / k, k - 1));
			double delta_s = (part_target_logp - logP[i]) / logp_per_sample;
			int sample_increase = (int)delta_s;
			samplesize[i] += sample_increase;
			for (size_t j = 0; j < L.size(); ++j) {
				size_t nu = L[j];
				double localec_accuracy = (s * k > nu) ? (pow(1.0 - nu / ((double)s * k), k - 1)) : 0;
				logP[j] += sample_increase * log(1.0 - nu / (2.0 * m) * localec_accuracy);
			}
		}
	}

	return { samplesize, L, };
}

std::tuple<std::vector<size_t>, std::vector<size_t>> exponential(double target_logp, size_t m, size_t k, size_t a, size_t alpha) {
	std::vector<size_t> samplesize;
	std::vector<size_t> L;
	std::vector<size_t> S;
	for (size_t s = 2; s <= a; s *= 2) {
		L.push_back(s);
		samplesize.push_back(0);
	}
	for (size_t i = 0; i < L.size(); i++) {
		const size_t& s = alpha * L[i];
		samplesize[i] = m / s;
		for (size_t j = 0; j < L.size(); ++j) {
			size_t nu = L[j];
			double localec_accuracy = (s * k > nu) ? (pow(1.0 - nu / ((double)s * k), k - 1)) : 0;
		}
	}

	return { samplesize, L, };
}

void optimtest() {
	/*
size_t k = 6;
size_t n = 100000;
size_t m = 2 * n * k;
// size_t a = pow(m, 1.0 / 3.0);
size_t a2 = pow(m, 2.0 / 3.0);

double target_p = 0.25; // probability of false negative
double target_logp = log(target_p);

std::vector<size_t> samplesize;
std::vector<size_t> L;
size_t alpha = 1;

//std::tie(samplesize, L) = high_vol_first(target_logp, m, k, a2);
//std::tie(samplesize, L) = low_vol_first(target_logp, m, k, a2);
//alpha = 8;std::tie(samplesize, L) = exponential(target_logp, m, k, a2, alpha);
//std::tie(samplesize, L) = high_vol_first_iterative(target_logp, m, k, a2);
{
	std::cout << "LP" << std::endl;
	std::tie(samplesize, L) = optim::optimise_sample_sizes_lp(target_p, m, k, a2);
	std::vector<double> logP = get_logP_vec(L, samplesize, k, m, alpha);
	size_t cost = 0;
	for (size_t j = 0; j < L.size(); ++j) {
		std::cout << "Samples at volume " << L[j] << ":\t" << samplesize[j] << "\t (" << exp(logP[j]) << " estimated probability of failure)" << endl;
		auto c = samplesize[j] * L[j] * alpha;
		std::cout << "Cost: " << c << std::endl;
		cost += c;
	}
	std::cout << "total cost: " << cost << std::endl;
}
{
	alpha = 8;
	std::cout << "exp (alpha = " << alpha << ")" << std::endl;
	std::tie(samplesize, L) = exponential(target_logp, m, k, a2, alpha);
	std::vector<double> logP = get_logP_vec(L, samplesize, k, m, alpha);
	size_t cost = 0;
	for (size_t j = 0; j < L.size(); ++j) {
		std::cout << "Samples at volume " << L[j] << " * " << alpha << ":\t" << samplesize[j] << "\t (" << exp(logP[j]) << " estimated probability of failure)" << endl;
		auto c = samplesize[j] * L[j];
		std::cout << "Cost: " << c << std::endl;
		cost += c;
	}
	std::cout << "total cost: " << cost << std::endl;
}
*/
}

void main() {
	test0();
}