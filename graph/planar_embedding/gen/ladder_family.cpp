// Prisms (planar) vs. Moebius ladders (nonplanar but sparse).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed % 3) {
		case 0: g = moebius_ladder(1000); break;       // No
		case 1: g = prism_graph(333333); break;        // Yes, M = 999999
		default: g = moebius_ladder(666666); break;    // No, M = 999999, passes Euler bound
	}
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
