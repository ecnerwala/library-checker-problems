// A single cycle on N_MAX vertices, shuffled.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	print_graph(gen, cycle_graph(N_MAX));
	return 0;
}
