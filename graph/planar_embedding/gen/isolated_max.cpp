// N = N_MAX isolated vertices, M = 0.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	Graph g; g.add_vertices(N_MAX);
	print_graph(gen, g);
	return 0;
}
