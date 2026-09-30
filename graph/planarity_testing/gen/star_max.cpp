// A star K_{1, N_MAX-1}, shuffled.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	print_graph(gen, star_graph(N_MAX));
	return 0;
}
