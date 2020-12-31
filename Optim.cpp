#include "pch.h"

#include "Optim.h"

#include <memory>
#include <functional>
#include "lp_lib.h"

// Pessimistic bound for the (logarithmic) probability.
inline double get_logP_bound(size_t nu, size_t s, size_t k, size_t m) {
	if (s * k <= nu)
		return 0; // Success is impossible at this call volume nu and cut size s.

	double localec_accuracy = pow(1.0 - nu / ((double)s * k), k - 1);
	double p = 1.0 - nu / (2.0 * m) * localec_accuracy;
	return log(p);
}

inline double get_logP(size_t nu, size_t s, size_t k, size_t m) {
	return get_logP_bound(nu, s, k, m);
}

std::pair<std::vector<size_t>, std::vector<size_t>> optim::optimise_sample_sizes_lp(double target_p, size_t m, size_t k, size_t a) {
	double target_logp = log(target_p);

	std::vector<size_t> S;          // S[i] is the i:th sample edge volume. (powers of two)
	std::vector<size_t> L;			// cut edge volumes. (powers of two)
	std::vector<size_t> samplesize; // samplesize[i] is the number of times we will call LocalEC with edge volume L[i]

	for (size_t s = 2; s <= a; s *= 2) {
		L.push_back(s);				// Powers of two between 2 and a
		samplesize.push_back(0);	// Initially no samples.
	}
	
	/*
	for (size_t s = 1; s <= 100 * a; s *= 2) {
		S.push_back(s);				// Powers of two in a larger range than 2 to a.
		if(s < 2 || s > a)
			samplesize.push_back(0);// Initially no samples.

		//size_t s2 = (s + 2 * s) / 2;
		//if (s2 != s) {
		//	S.push_back(s2);				// Values between the powers of two.
		//	samplesize.push_back(0);	// Initially no samples.
		//}
	}
	*/
	S = L; // Allowing a larger range of values does not seem to add value.

	int N1 = L.size();
	int N2 = S.size();

	std::unique_ptr<REAL[]> row(new REAL[N2 + 1]);
	lprec* lp = make_lp(0, N2);
	if (lp == NULL) throw 1;

	for (int j = 0; j < N1; j++) {
		int rowNo = j + 1;
		const size_t nu = L[j];
		for (int i = 0; i < N2; i++) {
			int colNo = i + 1;
			const size_t& s = S[i];
			row[colNo] = get_logP(nu, s, k, m);
		}

		if (!add_constraint(lp, row.get(), LE, target_logp)) throw 2;
	}

	//for(int colNo = 1; colNo <= N2; colNo++)
	//	set_int(lp, colNo, true);

	for (int i = 0; i < N2; ++i) {
		row[i + 1] = S[i];
	}
	if (!set_obj_fn(lp, row.get())) throw 3;
	set_minim(lp);

	//write_LP(lp, stdout); // Debug
	set_verbose(lp, IMPORTANT);
	//set_verbose(lp, DETAILED);

	if (solve(lp)) throw 4;

	REAL *sol;
	if (!get_ptr_primal_solution(lp, &sol)) throw 5;
	for (int i = 0; i < N2; i++)
		samplesize[i] = ceil(sol[i + N1 + 1]);

	delete_lp(lp);

	return { samplesize, S };
}