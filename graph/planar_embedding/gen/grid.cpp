// Grid graphs, with and without random holes.
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs = {
		grid_graph(gen, 3, 1000),
		grid_graph(gen, 300, 300),
		grid_graph(gen, 300, 300, 0.15),
	};
	print_graphs(gen, gs);
	return 0;
}
