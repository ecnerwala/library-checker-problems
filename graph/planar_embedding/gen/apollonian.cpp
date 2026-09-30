// Random Apollonian network (stacked triangulation, maximal planar, Yes).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int n = 333335;
	Graph g = random_apollonian(gen, n);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
