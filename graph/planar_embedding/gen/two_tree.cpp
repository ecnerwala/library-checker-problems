// Random 2-tree (series-parallel) plus two random extra edges (M = 2N - 1 passes the Euler bound).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g = random_two_tree(gen, 499999);
	add_random_nonedges(gen, g, 2);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
