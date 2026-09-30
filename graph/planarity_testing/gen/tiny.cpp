// Degenerate tiny graphs: N = 1, 2, 3 with and without edges.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	if (seed == 0) { g.add_vertices(1); }
	else if (seed == 1) { g.add_vertices(2); }
	else if (seed == 2) { g.add_vertices(3); }
	else if (seed == 3) { g = path_graph(3); }
	else { g = cycle_graph(3); }
	print_graph(gen, g);
	return 0;
}
