// Ladders, prisms and Moebius ladders (Moebius ladders are sparse nonplanar graphs that pass the Euler bound).
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs = {
		moebius_ladder(8), moebius_ladder(1000),   // No
		ladder_graph(20000), prism_graph(20000),   // Yes
		moebius_ladder(40000),                     // No
	};
	print_graphs(gen, gs);
	return 0;
}
