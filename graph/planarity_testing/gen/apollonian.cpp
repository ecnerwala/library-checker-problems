// Random Apollonian networks (stacked triangulations, maximal planar, Yes);
// odd seeds add one random non-edge (No).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int ns[] = {50, 5000, 333335, 333334};
	int n = ns[seed % 4];
	Graph g = random_apollonian(gen, n);
	if (seed % 2 == 1) add_random_nonedges(gen, g, 1);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
