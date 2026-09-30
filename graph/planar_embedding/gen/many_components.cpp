// Many disjoint copies of small graphs.
//   0: many copies of K5 (No)
//   1: random small planar components + exactly one K3,3 (No)
//   2: many random small triangulations (Yes)
#include "planar_gen.h"
#include "../params.h"

Graph components(Random& gen, int mode, int n_budget, int m_budget) {
	Graph g;
	switch (mode) {
		case 0: while (g.n + 5 <= n_budget && int(g.edges.size()) + 10 <= m_budget) g.add_graph(complete_graph(5)); break;
		case 1: {
			int bad_at = gen.uniform(0, n_budget / 20);
			for (int i = 0; g.n + 40 <= n_budget && int(g.edges.size()) + 100 <= m_budget; i++) {
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
			while (g.n + 30 <= n_budget && int(g.edges.size()) + 90 <= m_budget) g.add_graph(random_triangulation(gen, gen.uniform(3, 30), 200));
			break;
	}
	return g;
}

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs;
	for (int mode = 0; mode < 3; mode++) gs.push_back(components(gen, mode, 50000, 150000));
	print_graphs(gen, gs);
	return 0;
}
