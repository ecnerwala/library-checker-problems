// Random planar triangulations (fan + random edge flips):
//   mode 0: maximal planar (Yes, M = 3N - 6)
//   mode 1: plus one random non-edge (No)
//   mode 2: random edge subset (Yes)
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int mode = seed % 3;
	int ns[] = {10, 1000, 333334};
	int n = ns[(seed / 3) % 3];
	Graph g = random_triangulation(gen, n, 4LL * n);
	if (mode == 1) add_random_nonedges(gen, g, 1);
	if (mode == 2) g = random_edge_subset(gen, g, gen.uniform01() * 0.6 + 0.3);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
