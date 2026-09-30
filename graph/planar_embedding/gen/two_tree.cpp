// Random 2-trees (Yes), and 2-trees plus two random non-edges (usually No; M = 2N - 1 passes the Euler bound).
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs;
	for (int n : {1000, 100000}) {
		Graph g = random_two_tree(gen, n);
		gs.push_back(g);
		add_random_nonedges(gen, g, 2);
		gs.push_back(g);
	}
	print_graphs(gen, gs);
	return 0;
}
