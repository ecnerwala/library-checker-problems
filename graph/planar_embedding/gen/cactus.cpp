// Random cactus graphs (Yes).
#include "planar_gen.h"

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	print_graphs(gen, {random_cactus(gen, 5000, 20), random_cactus(gen, 100000, 10)});
	return 0;
}
