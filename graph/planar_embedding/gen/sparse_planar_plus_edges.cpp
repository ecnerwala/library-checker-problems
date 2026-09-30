// A random sparse planar graph (random subset of a triangulation, or a 2-tree)
// plus a few random extra edges: usually No, sometimes Yes; the Euler bound never fires.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	std::pair<int, int> nk[] = {{1000, 2}, {50000, 1}, {400000, 1}, {400000, 3}};
	auto [n, k] = nk[seed % 4];
	Graph g;
	if (seed % 2 == 0) g = random_edge_subset(gen, random_triangulation(gen, n, 3LL * n), 0.5);
	else g = random_edge_subset(gen, random_two_tree(gen, n), 0.9);
	add_random_nonedges(gen, g, k);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
