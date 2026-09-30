// Random cactus graphs (Yes).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	std::pair<int, int> nc[] = {{5000, 20}, {666666, 10}};
	auto [n, c] = nc[seed % 2];
	Graph g = random_cactus(gen, n, c);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g);
	return 0;
}
