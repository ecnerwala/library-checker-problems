// Classic small named graphs (relabeled and shuffled).
#include "planar_gen.h"
#include "../params.h"

Graph minus_edge(Graph g, int idx) { g.edges.erase(g.edges.begin() + idx); return g; }
Graph minus_vertex(Graph g, int v) {
	Graph h; h.add_vertices(g.n - 1);
	for (auto [a, b] : g.edges) {
		if (a == v || b == v) continue;
		h.add_edge(a - (a > v), b - (b > v));
	}
	return h;
}

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	Graph g;
	switch (seed) {
		case 0: g = complete_graph(5); break;                     // No
		case 1: g = complete_bipartite(3, 3); break;              // No
		case 2: g = petersen_graph(); break;                      // No (no K5 / K3,3 subgraph)
		case 3: g = moebius_ladder(8); break;                     // Wagner graph V8, No
		case 4: g = moebius_ladder(10); break;                    // No
		case 5: g = goldner_harary_graph(); break;                // Yes (maximal planar)
		case 6: g = grotzsch_graph(); break;                      // No
		case 7: g = wheel_graph(7); break;                        // Yes
		case 8: g = prism_graph(5); break;                        // Yes
		case 9: g = minus_edge(complete_graph(5), gen.uniform(0, 9)); break;         // K5 - e, Yes
		case 10: g = minus_edge(complete_bipartite(3, 3), gen.uniform(0, 8)); break; // K3,3 - e, Yes
		case 11: g = minus_edge(complete_graph(6), gen.uniform(0, 14)); break;       // K6 - e, No
		case 12: g = complete_graph(7); break;                    // No
		case 13: g = complete_bipartite(2, 5); break;             // Yes
		case 14: g = complete_bipartite(3, 4); break;             // No
		case 15: g = complete_bipartite(4, 4); break;             // No
		case 16: g = icosahedron_graph(); break;                  // Yes
		case 17: g = dodecahedron_graph(); break;                 // Yes
		case 18: g = octahedron_graph(); break;                   // Yes
		case 19: g = heawood_graph(); break;                      // No
		case 20: g = hypercube_graph(3); break;                   // Yes
		case 21: g = hypercube_graph(4); break;                   // No
		case 22: g = minus_edge(petersen_graph(), gen.uniform(0, 14)); break;        // Petersen - e
		case 23: g = minus_vertex(petersen_graph(), gen.uniform(0, 9)); break;       // Petersen - v, No
		default: g = complete_graph(4); break;                    // Yes
	}
	print_graph(gen, g);
	return 0;
}
