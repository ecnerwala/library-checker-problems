// A long cycle with a random laminar family of chords drawn inside and another
// drawn outside (nested and interleaved chords, Yes; M ~ 3N). Odd seeds add
// one more random chord (the correct solution decides).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int ns[] = {2000, 333333};
	int n = ns[(seed / 2) % 2];
	Graph g = random_maximal_outerplanar(gen, n);
	Graph outside = random_maximal_outerplanar(gen, n);
	for (size_t i = n; i < outside.edges.size(); i++) g.edges.push_back(outside.edges[i]);
	dedup(g);
	if (seed % 2 == 1) add_random_nonedges(gen, g, 1);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
