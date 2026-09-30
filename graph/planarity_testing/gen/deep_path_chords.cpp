// A path on N_MAX - 100 vertices in natural order plus k random chords (deep DFS
// tree with a few back edges; the correct solution decides).
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int ks[] = {1, 3, 50};
	int n = N_MAX - 100;
	Graph g = path_graph(n);
	Edges chords;
	std::unordered_set<long long> seen;
	while (int(chords.size()) < ks[seed % 3]) {
		auto [u, v] = gen.uniform_pair(0, n - 1);
		if (v - u <= 1 && u - v <= 1) continue;
		if (seen.insert(edge_key(u, v)).second) chords.emplace_back(u, v);
	}
	for (auto e : chords) g.edges.push_back(e);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g, false, false, false);
	return 0;
}
