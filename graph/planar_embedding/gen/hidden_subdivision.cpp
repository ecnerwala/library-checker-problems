// A subdivision of K5 or K3,3 with long subdivided paths, hidden inside a huge
// planar host graph (No, sparse: defeats the Euler bound).
//   seed % 4: 0: K5 glued to a random deep tree (random labels)
//             1: K3,3 as a separate component, host a random deep tree (random labels)
//             2: K5 as a separate component, host a long path (natural labels)
//             3: K3,3 glued to a long path (natural labels)
#include "planar_gen.h"
#include "../params.h"

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int mode = seed % 4;
	bool k33 = mode % 2 == 1, separate = mode == 1 || mode == 2, path_host = mode >= 2;
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
