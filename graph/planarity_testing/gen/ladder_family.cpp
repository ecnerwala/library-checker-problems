// Ladders, prisms and wheels (planar) vs. Moebius ladders (nonplanar but sparse).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed % 6) {
		case 0: g = prism_graph(1000); break;          // Yes
		case 1: g = ladder_graph(1000); break;         // Yes
		case 2: g = moebius_ladder(1000); break;       // No
		case 3: g = prism_graph(333333); break;        // Yes, M = 999999
		case 4: g = moebius_ladder(666666); break;     // No, M = 999999, passes Euler bound
		default: g = wheel_graph(500001); break;       // Yes, M = M_MAX
	}
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
