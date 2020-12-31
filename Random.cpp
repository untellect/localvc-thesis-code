#include "pch.h"

#include "Random.h"

/*
 * A random unsigned integer between 0 and max (inclusive)
 */
size_t random_int(size_t min, size_t max) {
    std::uniform_int_distribution<size_t> dist(min, max);
    return dist(random_engine);
}

/*
 * Sample the number of successful attempts if we try until we fail and each attempt fails with probability fail_prob.
 */
size_t successes_until_failure(double fail_prob) {
    std::negative_binomial_distribution<int> dist(1, fail_prob);
    return dist(random_engine);
}
