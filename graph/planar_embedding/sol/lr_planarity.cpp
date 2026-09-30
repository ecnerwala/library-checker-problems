// Independent cross-check: iterative left-right planarity test and embedding
// (U. Brandes, "The Left-Right Planarity Test", 2009), following the
// structure of networkx's networkx/algorithms/planarity.py.
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

namespace {

struct Interval {
	int low = -1, high = -1;
	bool empty() const { return low == -1 && high == -1; }
};
struct ConflictPair {
	Interval L, R;
	int id;
};

struct LRPlanarity {
	int N, M;
	std::vector<std::array<int, 2>> edges;
	std::vector<int> adj_start, adj_edge; // undirected CSR: adj_edge holds edge ids
	std::vector<int> height, parent_edge;
	std::vector<int> from, to;
	std::vector<char> oriented;
	std::vector<int> lowpt, lowpt2, nesting_depth;
	std::vector<int> out_start, out_edge; // DFS-oriented out-edges, sorted by nesting_depth
	std::vector<int> ref, lowpt_edge, stack_bottom, side;
	std::vector<ConflictPair> S;
	int next_id = 0;

	LRPlanarity(int N_, std::vector<std::array<int, 2>> edges_) : N(N_), M(int(edges_.size())), edges(std::move(edges_)) {}

	int top_id() const { return S.empty() ? -1 : S.back().id; }

	void orient() {
		adj_start.assign(N + 1, 0);
		for (auto [u, v] : edges) { adj_start[u + 1]++; adj_start[v + 1]++; }
		for (int i = 0; i < N; i++) adj_start[i + 1] += adj_start[i];
		adj_edge.resize(2 * M);
		{
			std::vector<int> pos(adj_start.begin(), adj_start.end() - 1);
			for (int e = 0; e < M; e++) {
				adj_edge[pos[edges[e][0]]++] = e;
				adj_edge[pos[edges[e][1]]++] = e;
			}
		}
		height.assign(N, -1);
		parent_edge.assign(N, -1);
		from.assign(M, -1); to.assign(M, -1);
		oriented.assign(M, 0);
		lowpt.assign(M, 0); lowpt2.assign(M, 0); nesting_depth.assign(M, 0);
		std::vector<int> out_cnt(N + 1, 0);
		std::vector<int> ind(N), pending(N, -1);
		std::vector<int> st;
		st.reserve(N);
		auto post = [&](int v, int ei) -> void {
			nesting_depth[ei] = 2 * lowpt[ei];
			if (lowpt2[ei] < height[v]) nesting_depth[ei] += 1;
			int e = parent_edge[v];
			if (e != -1) {
				if (lowpt[ei] < lowpt[e]) {
					lowpt2[e] = std::min(lowpt[e], lowpt2[ei]);
					lowpt[e] = lowpt[ei];
				} else if (lowpt[ei] > lowpt[e]) {
					lowpt2[e] = std::min(lowpt2[e], lowpt[ei]);
				} else {
					lowpt2[e] = std::min(lowpt2[e], lowpt2[ei]);
				}
			}
		};
		for (int r = 0; r < N; r++) {
			if (height[r] != -1) continue;
			height[r] = 0;
			ind[r] = adj_start[r];
			st.push_back(r);
			while (!st.empty()) {
				int v = st.back();
				if (pending[v] != -1) {
					post(v, pending[v]);
					pending[v] = -1;
					ind[v]++;
				}
				bool descended = false;
				while (ind[v] < adj_start[v + 1]) {
					int ei = adj_edge[ind[v]];
					if (oriented[ei]) { ind[v]++; continue; }
					int w = edges[ei][0] ^ edges[ei][1] ^ v;
					oriented[ei] = 1;
					from[ei] = v; to[ei] = w;
					out_cnt[v + 1]++;
					lowpt[ei] = lowpt2[ei] = height[v];
					if (height[w] == -1) {
						parent_edge[w] = ei;
						height[w] = height[v] + 1;
						ind[w] = adj_start[w];
						pending[v] = ei;
						st.push_back(w);
						descended = true;
						break;
					}
					lowpt[ei] = height[w];
					post(v, ei);
					ind[v]++;
				}
				if (!descended) st.pop_back();
			}
		}
		// Stable counting sort of oriented edges by nesting_depth, then bucket by source.
		std::vector<int> by_depth(M);
		{
			int D = 2 * N + 2;
			std::vector<int> cnt(D + 1, 0);
			for (int e = 0; e < M; e++) cnt[nesting_depth[e] + 1]++;
			for (int i = 0; i < D; i++) cnt[i + 1] += cnt[i];
			for (int e = 0; e < M; e++) by_depth[cnt[nesting_depth[e]]++] = e;
		}
		for (int i = 0; i < N; i++) out_cnt[i + 1] += out_cnt[i];
		out_start = out_cnt;
		out_edge.resize(M);
		{
			std::vector<int> pos(out_start.begin(), out_start.end() - 1);
			for (int e : by_depth) out_edge[pos[from[e]]++] = e;
		}
	}

	bool conflicting(const Interval& I, int b) const {
		return !I.empty() && lowpt[I.high] > lowpt[b];
	}
	int lowest(const ConflictPair& P) const {
		if (P.L.empty()) return lowpt[P.R.low];
		if (P.R.empty()) return lowpt[P.L.low];
		return std::min(lowpt[P.L.low], lowpt[P.R.low]);
	}

	bool add_constraints(int ei, int e) {
		ConflictPair P{Interval{}, Interval{}, next_id++};
		// merge return edges of e_i into P.R
		while (true) {
			assert(!S.empty());
			ConflictPair Q = S.back(); S.pop_back();
			if (!Q.L.empty()) std::swap(Q.L, Q.R);
			if (!Q.L.empty()) return false;
			if (lowpt[Q.R.low] > lowpt[e]) {
				if (P.R.empty()) P.R = Q.R;
				else ref[P.R.low] = Q.R.high;
				P.R.low = Q.R.low;
			} else {
				ref[Q.R.low] = lowpt_edge[e];
			}
			if (top_id() == stack_bottom[ei]) break;
		}
		// merge conflicting return edges of e_1, ..., e_{i-1} into P.L
		while (!S.empty() && (conflicting(S.back().L, ei) || conflicting(S.back().R, ei))) {
			ConflictPair Q = S.back(); S.pop_back();
			if (conflicting(Q.R, ei)) std::swap(Q.L, Q.R);
			if (conflicting(Q.R, ei)) return false;
			ref[P.R.low] = Q.R.high;
			if (Q.R.low != -1) P.R.low = Q.R.low;
			if (P.L.empty()) P.L = Q.L;
			else ref[P.L.low] = Q.L.high;
			P.L.low = Q.L.low;
		}
		if (!(P.L.empty() && P.R.empty())) S.push_back(P);
		return true;
	}

	void remove_back_edges(int e) {
		int u = from[e];
		while (!S.empty() && lowest(S.back()) == height[u]) {
			ConflictPair P = S.back(); S.pop_back();
			if (P.L.low != -1) side[P.L.low] = -1;
		}
		if (!S.empty()) {
			ConflictPair P = S.back(); S.pop_back();
			while (P.L.high != -1 && to[P.L.high] == u) P.L.high = ref[P.L.high];
			if (P.L.high == -1 && P.L.low != -1) {
				ref[P.L.low] = P.R.low;
				side[P.L.low] = -1;
				P.L.low = -1;
			}
			while (P.R.high != -1 && to[P.R.high] == u) P.R.high = ref[P.R.high];
			if (P.R.high == -1 && P.R.low != -1) {
				ref[P.R.low] = P.L.low;
				side[P.R.low] = -1;
				P.R.low = -1;
			}
			S.push_back(P);
		}
		if (lowpt[e] < height[u]) {
			int hl = S.back().L.high, hr = S.back().R.high;
			if (hl != -1 && (hr == -1 || lowpt[hl] > lowpt[hr])) ref[e] = hl;
			else ref[e] = hr;
		}
	}

	bool test() {
		ref.assign(M, -1);
		side.assign(M, 1);
		lowpt_edge.assign(M, -1);
		stack_bottom.assign(M, -1);
		S.clear();
		S.reserve(M);
		std::vector<int> ind(N), pending(N, -1);
		std::vector<int> st;
		st.reserve(N);
		auto integrate = [&](int v, int ei) -> bool {
			if (lowpt[ei] < height[v]) {
				int e = parent_edge[v];
				assert(e != -1);
				if (ei == out_edge[out_start[v]]) lowpt_edge[e] = lowpt_edge[ei];
				else if (!add_constraints(ei, e)) return false;
			}
			return true;
		};
		for (int r = 0; r < N; r++) {
			if (parent_edge[r] != -1) continue;
			ind[r] = out_start[r];
			st.push_back(r);
			while (!st.empty()) {
				int v = st.back();
				if (pending[v] != -1) {
					int ei = pending[v];
					pending[v] = -1;
					if (!integrate(v, ei)) return false;
					ind[v]++;
				}
				bool descended = false;
				while (ind[v] < out_start[v + 1]) {
					int ei = out_edge[ind[v]];
					int w = to[ei];
					stack_bottom[ei] = top_id();
					if (ei == parent_edge[w]) {
						ind[w] = out_start[w];
						pending[v] = ei;
						st.push_back(w);
						descended = true;
						break;
					}
					lowpt_edge[ei] = ei;
					S.push_back(ConflictPair{Interval{}, Interval{ei, ei}, next_id++});
					if (!integrate(v, ei)) return false;
					ind[v]++;
				}
				if (!descended) {
					if (parent_edge[v] != -1) remove_back_edges(parent_edge[v]);
					st.pop_back();
				}
			}
		}
		return true;
	}

	// Resolve the relative sides of the return edges (networkx: sign()).
	void resolve_sides() {
		std::vector<int> chain;
		for (int e0 = 0; e0 < M; e0++) {
			if (ref[e0] == -1) continue;
			chain.clear();
			int e = e0;
			while (ref[e] != -1) {
				chain.push_back(e);
				e = ref[e];
			}
			for (int i = int(chain.size()) - 1; i >= 0; i--) {
				int c = chain[i];
				side[c] *= side[ref[c]];
				ref[c] = -1;
			}
		}
		for (int e = 0; e < M; e++) nesting_depth[e] *= side[e];
	}

	// Sort the out-edges of every vertex by (signed) nesting depth.
	void sort_out_edges() {
		int D = 2 * N + 2;
		std::vector<int> by_depth(M);
		{
			std::vector<int> cnt(2 * D + 2, 0);
			for (int e = 0; e < M; e++) cnt[nesting_depth[e] + D + 1]++;
			for (int i = 0; i < 2 * D + 1; i++) cnt[i + 1] += cnt[i];
			for (int e = 0; e < M; e++) by_depth[cnt[nesting_depth[e] + D]++] = e;
		}
		std::vector<int> pos(out_start.begin(), out_start.end() - 1);
		for (int e : by_depth) out_edge[pos[from[e]]++] = e;
	}

	// Darts: 2e leaves from[e], 2e+1 leaves to[e]. cw/ccw are the cyclic
	// rotation lists around each vertex (networkx: dfs_embedding()).
	std::vector<int> cw, ccw, first_dart;
	int dart_neighbor(int d) const { return d & 1 ? from[d >> 1] : to[d >> 1]; }
	void insert_cw_of(int d, int ref_d) { // d becomes the cw neighbor of ref_d
		int nx = cw[ref_d];
		ccw[d] = ref_d; cw[d] = nx;
		cw[ref_d] = d; ccw[nx] = d;
	}
	void insert_ccw_of(int d, int ref_d) { // d becomes the ccw neighbor of ref_d
		int pv = ccw[ref_d];
		cw[d] = ref_d; ccw[d] = pv;
		ccw[ref_d] = d; cw[pv] = d;
	}

	void embed() {
		resolve_sides();
		sort_out_edges();
		cw.assign(2 * M, -1);
		ccw.assign(2 * M, -1);
		first_dart.assign(N, -1);
		// initialize each rotation with the out-edges in sorted (clockwise) order
		for (int v = 0; v < N; v++) {
			int lo = out_start[v], hi = out_start[v + 1];
			if (lo == hi) continue;
			for (int i = lo; i < hi; i++) {
				int d = 2 * out_edge[i];
				int dn = 2 * out_edge[i + 1 == hi ? lo : i + 1];
				cw[d] = dn;
				ccw[dn] = d;
			}
			first_dart[v] = 2 * out_edge[lo]; // leftmost neighbor
		}
		std::vector<int> left_ref(N, -1), right_ref(N, -1);
		std::vector<int> ind(N);
		std::vector<int> st;
		st.reserve(N);
		for (int r = 0; r < N; r++) {
			if (parent_edge[r] != -1) continue;
			ind[r] = out_start[r];
			st.push_back(r);
			while (!st.empty()) {
				int v = st.back();
				bool descended = false;
				while (ind[v] < out_start[v + 1]) {
					int ei = out_edge[ind[v]++];
					int w = to[ei];
					if (ei == parent_edge[w]) {
						// (w -> v) becomes the leftmost neighbor of w
						int d = 2 * ei + 1;
						if (first_dart[w] == -1) {
							cw[d] = ccw[d] = d;
						} else {
							insert_ccw_of(d, first_dart[w]);
						}
						first_dart[w] = d;
						left_ref[v] = right_ref[v] = 2 * ei;
						ind[w] = out_start[w];
						st.push_back(w);
						descended = true;
						break;
					}
					int d = 2 * ei + 1; // (w -> v) for the back edge v -> w
					if (side[ei] == 1) {
						insert_cw_of(d, right_ref[w]);
					} else {
						insert_ccw_of(d, left_ref[w]);
						left_ref[w] = d;
					}
				}
				if (!descended) st.pop_back();
			}
		}
	}

	bool is_planar() {
		orient();
		return test();
	}
};

} // namespace

int main() {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	int T;
	std::cin >> T;
	while (T--) {
		int N, M;
		std::cin >> N >> M;
		std::vector<std::array<int, 2>> edges(M);
		for (auto& e : edges) {
			std::cin >> e[0] >> e[1];
		}
		LRPlanarity lr(N, std::move(edges));
		if (!lr.is_planar()) {
			std::cout << "No\n";
			continue;
		}
		lr.embed();
		std::cout << "Yes\n";
		for (int v = 0; v < N; v++) {
			int d0 = lr.first_dart[v];
			if (d0 != -1) {
				int d = d0;
				bool first = true;
				do {
					if (!first) std::cout << ' ';
					first = false;
					std::cout << lr.dart_neighbor(d);
					d = lr.cw[d];
				} while (d != d0);
			}
			std::cout << '\n';
		}
	}
	return 0;
}
