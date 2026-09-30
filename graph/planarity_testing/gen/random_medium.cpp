// Random simple graphs with M around N (near the planarity threshold for random graphs).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int ns[] = {50, 200, 1000, 5000, 20000, 100000};
	double fs[] = {0.7, 0.9, 1.0, 1.1, 1.3, 1.6};
	int n = ns[gen.uniform(0, 5)];
	int m = int(n * fs[gen.uniform(0, 5)]);
	print_graph(gen, random_simple_graph(gen, n, m));
	return 0;
}
