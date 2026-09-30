// Random maximal outerplanar graphs (cycle + non-crossing chords), random
// subsets of them (Yes), and maximal outerplanar graphs plus one random chord
// (also Yes: the extra edge can be drawn in the outer face).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed % 6) {
		case 0: g = random_maximal_outerplanar(gen, 20); break;
		case 1: g = random_edge_subset(gen, random_maximal_outerplanar(gen, 1000), 0.8); break;
		case 2: g = random_maximal_outerplanar(gen, 100000); break;
		case 3: g = random_maximal_outerplanar(gen, 500001); break;   // M = 999999
		case 4: g = random_maximal_outerplanar(gen, 1000); add_random_nonedges(gen, g, 1); break;
		default: g = random_maximal_outerplanar(gen, 500000); add_random_nonedges(gen, g, 1); break;
	}
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
