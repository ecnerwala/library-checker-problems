// Random maximal outerplanar graphs (Yes), and the same plus one random non-edge (usually No).
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs;
	for (int n : {1000, 100000}) {
		Graph g = random_maximal_outerplanar(gen, n);
		gs.push_back(g);
		add_random_nonedges(gen, g, 1);
		gs.push_back(g);
	}
	print_graphs(gen, gs);
	return 0;
}
