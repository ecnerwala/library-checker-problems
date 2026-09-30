// Complete bipartite graphs K_{a,b}: planar iff min(a, b) <= 2.
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	std::pair<int, int> ab[] = {
		{3, 300}, {2, 3000}, {1000, 1000},
		{2, 500000},   // Yes, M = M_MAX
		{3, 333333},   // No, M = 999999
		{5, 200000},   // No, M = M_MAX
	};
	auto [a, b] = ab[seed % 6];
	assert((long long)a * b <= M_MAX && a + b <= N_MAX);
	print_graph(gen, complete_bipartite(a, b));
	return 0;
}
