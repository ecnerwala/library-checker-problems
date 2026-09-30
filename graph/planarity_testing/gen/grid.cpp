// Grid graphs, optionally with random holes.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed % 4) {
		case 0: g = grid_graph(gen, 3, 1000); break;
		case 1: g = grid_graph(gen, 707, 707); break;          // M = 998364
		case 2: g = grid_graph(gen, 2, 333333); break;         // M = 999997
		default: g = grid_graph(gen, 700, 700, 0.15); break;
	}
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
