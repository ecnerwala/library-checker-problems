// Random simple graphs on N_MAX vertices with M = 10^6, 5 * 10^5, 10^5 edges.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int ms[] = {M_MAX, 500000, 100000};
	print_graph(gen, random_simple_graph(gen, N_MAX, ms[seed % 3]));
	return 0;
}
