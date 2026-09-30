// Uniformly random simple graphs with 5 <= N <= 12 at random densities.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int n = gen.uniform(5, 12);
	int maxm = n * (n - 1) / 2;
	int m = gen.uniform_bool() ? gen.uniform(0, maxm) : gen.uniform(std::min(maxm, n), std::min(maxm, 3 * n));
	print_graph(gen, random_simple_graph(gen, n, m));
	return 0;
}
