// A single path on N_MAX vertices (DFS depth 10^6). Seed 0: natural labels and
// edge order; seed 1: shuffled.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g = path_graph(N_MAX);
	bool shuf = seed % 2 == 1;
	print_graph(gen, g, shuf, shuf, shuf);
	return 0;
}
