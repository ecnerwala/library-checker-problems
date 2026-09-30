// Random Apollonian networks (planar 3-trees, maximal planar).
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	print_graphs(gen, {random_apollonian(gen, 1000), random_apollonian(gen, 100000)});
	return 0;
}
