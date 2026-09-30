// Sparse random planar graphs (edge subsets of triangulations / 2-trees) plus a few random extra edges
// (usually No; passes the Euler bound).
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs;
	std::pair<int, int> nk[] = {{1000, 2}, {50000, 1}, {100000, 1}, {100000, 3}};
	for (int i = 0; i < 4; i++) {
		auto [n, k] = nk[i];
		Graph g;
		if (i % 2 == 0) g = random_edge_subset(gen, random_triangulation(gen, n, int64_t(3) * n), 0.5);
		else g = random_edge_subset(gen, random_two_tree(gen, n), 0.9);
		add_random_nonedges(gen, g, k);
		gs.push_back(g);
	}
	print_graphs(gen, gs);
	return 0;
}
