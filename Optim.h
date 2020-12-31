#pragma once

#include <vector>
#include <tuple>

namespace optim {
	std::pair<std::vector<size_t>, std::vector<size_t> > optimise_sample_sizes_lp(double target_p, size_t m, size_t k, size_t a);
}
