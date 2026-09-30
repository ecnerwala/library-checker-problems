// Many disjoint copies of small graphs.
//   0: 166666 x K4 (Yes)        1: 100000 x K5 (No)         2: 111111 x K3,3 (No)
//   3: 333333 x triangle (Yes)  4: random small planar components + exactly one K3,3 (No)
//   5: many random small triangulations (Yes)
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed % 4) {
		case 0: for (int i = 0; i < 100000; i++) g.add_graph(complete_graph(5)); break;
		case 1: for (int i = 0; i < 111111; i++) g.add_graph(complete_bipartite(3, 3)); break;
		case 2: {
			int bad_at = gen.uniform(0, 100000);
			for (int i = 0; i <= 100000 && g.n + 40 <= N_MAX && int(g.edges.size()) + 100 <= M_MAX; i++) {
				if (i == bad_at) { g.add_graph(complete_bipartite(3, 3)); continue; }
				switch (gen.uniform(0, 3)) {
					case 0: g.add_graph(random_tree(gen, gen.uniform(1, 10))); break;
					case 1: g.add_graph(random_two_tree(gen, gen.uniform(2, 10))); break;
					case 2: g.add_graph(complete_bipartite(2, gen.uniform(1, 8))); break;
					default: g.add_graph(random_maximal_outerplanar(gen, gen.uniform(3, 12))); break;
				}
			}
			break;
		}
		default:
			while (g.n + 30 <= N_MAX && int(g.edges.size()) + 90 <= M_MAX) g.add_graph(random_triangulation(gen, gen.uniform(3, 30), 200));
			break;
	}
	assert(g.n <= N_MAX && int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
