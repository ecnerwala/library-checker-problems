// A subdivision of K5 or K3,3 with long subdivided paths, hidden inside a huge
// planar host graph (No, sparse: defeats the Euler bound).
//   bit 0: K5 (0) or K3,3 (1)
//   bit 1: attached to the host (0) or a separate component not containing vertex 0 (1)
//   bit 2: host is a random deep tree with random labels (0) or a long path with natural labels (1)
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	bool k33 = seed & 1, separate = seed >> 1 & 1, path_host = seed >> 2 & 1;
	Graph core = k33 ? complete_bipartite(3, 3) : complete_graph(5);
	Graph sub = subdivide_edges(gen, core, 10000, 30000);
	int host_n = std::min(N_MAX - sub.n, M_MAX - int(sub.edges.size()));
	Graph g = path_host ? path_graph(host_n) : random_deep_tree(gen, host_n, 3);
	int off = g.add_graph(sub);
	if (!separate) {
		// Glue: connect a random vertex of the subdivision to a random host vertex.
		g.add_edge(gen.uniform(0, host_n - 1), off + gen.uniform(0, sub.n - 1));
	}
	assert(g.n <= N_MAX && int(g.edges.size()) <= M_MAX);
	if (path_host) {
		print_graph(gen, g, false, false, false);
	} else {
		// Random labels, but vertex 0 must stay in the host.
		auto perm = gen.perm<int>(g.n);
		int host_v = gen.uniform(0, host_n - 1);
		for (int i = 0; i < g.n; i++) if (perm[i] == 0) { std::swap(perm[i], perm[host_v]); break; }
		for (auto& [u, v] : g.edges) { u = perm[u]; v = perm[v]; }
		print_graph(gen, g, false, true, true);
	}
	return 0;
}
