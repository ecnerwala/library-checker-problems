// Many uniformly random simple graphs with 5 <= N <= 12 per file, at random
// densities. Even seeds: all densities; odd seeds: densities around the
// planarity threshold (N <= M <= 3N).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	std::vector<Graph> gs;
	for (int t = 0; t < 5000; t++) {
		int n = gen.uniform(5, 12);
		int maxm = n * (n - 1) / 2;
		int m = seed % 2 == 0 ? gen.uniform(0, maxm) : gen.uniform(std::min(maxm, n), std::min(maxm, 3 * n));
		gs.push_back(random_simple_graph(gen, n, m));
	}
	print_graphs(gen, gs);
	return 0;
}
