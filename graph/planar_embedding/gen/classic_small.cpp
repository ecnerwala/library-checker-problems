// Classic small named graphs (relabeled and shuffled).
#include <cassert>
#include "planar_gen.h"

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
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs;
	for (int seed = 0; seed < 18; seed++) {
	Graph g;
	switch (seed) {
		case 0: g = petersen_graph(); break;                      // No (no K5 / K3,3 subgraph)
		case 1: g = moebius_ladder(8); break;                     // Wagner graph V8, No
		case 2: g = moebius_ladder(10); break;                    // No
		case 3: g = goldner_harary_graph(); break;                // Yes (maximal planar)
		case 4: g = grotzsch_graph(); break;                      // No
		case 5: g = wheel_graph(7); break;                        // Yes
		case 6: g = prism_graph(5); break;                        // Yes
		case 7: g = minus_edge(complete_graph(5), gen.uniform(0, 9)); break;         // K5 - e, Yes
		case 8: g = minus_edge(complete_bipartite(3, 3), gen.uniform(0, 8)); break; // K3,3 - e, Yes
		case 9: g = minus_edge(complete_graph(6), gen.uniform(0, 14)); break;       // K6 - e, No
		case 10: g = icosahedron_graph(); break;                  // Yes
		case 11: g = dodecahedron_graph(); break;                 // Yes
		case 12: g = octahedron_graph(); break;                   // Yes
		case 13: g = heawood_graph(); break;                      // No
		case 14: g = hypercube_graph(3); break;                   // Yes
		case 15: g = hypercube_graph(4); break;                   // No
		case 16: g = minus_edge(petersen_graph(), gen.uniform(0, 14)); break;        // Petersen - e
		case 17: g = minus_vertex(petersen_graph(), gen.uniform(0, 9)); break;       // Petersen - v, No
		default: assert(false);
	}
	gs.push_back(g);
	}
	print_graphs(gen, gs);
	return 0;
}
