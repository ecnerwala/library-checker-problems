// Complete graphs K_n; n = 1414 gives M = 998991.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int ns[] = {6, 8, 50, 1414};
	int n = ns[seed % 4];
	assert((long long)n * (n - 1) / 2 <= M_MAX);
	print_graph(gen, complete_graph(n));
	return 0;
}
