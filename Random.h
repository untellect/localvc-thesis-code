#pragma once

#include <vector>
#include <random>

static std::default_random_engine random_engine(time(NULL));

/*
 * Randomly shuffle a vector of something.
 */
template<typename T>
void random_shuffle(std::vector<T>& vec) {
	std::shuffle(vec.begin(), vec.end(), random_engine);
}

/*
 * A random unsigned integer between 0 and max (inclusive)
 */
size_t random_int(size_t min, size_t max);

/*
 * Sample the number of successful attempts if we try until we fail and each attempt fails with probability fail_prob.
 */
size_t successes_until_failure(double fail_prob);

// TODO wrap all calls to <random> here so we can seed stuff properly