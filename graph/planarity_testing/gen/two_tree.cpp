// Random 2-trees (series-parallel, Yes) and 2-trees plus two random extra edges.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed % 4) {
		case 0: g = random_two_tree(gen, 20); break;
		case 1: g = random_two_tree(gen, 10000); add_random_nonedges(gen, g, 2); break;
		case 2: g = random_two_tree(gen, 500001); break;   // M = 999999
		default: g = random_two_tree(gen, 499999); add_random_nonedges(gen, g, 2); break;
	}
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
