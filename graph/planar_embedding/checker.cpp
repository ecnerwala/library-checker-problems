#include <algorithm>
#include <cstdint>
#include <numeric>
#include <string>
#include <vector>
#include "testlib.h"

using namespace std;

struct Graph {
  int n, m;
  vector<int> start;  // CSR offsets, size n + 1
  vector<int> nb;     // nb[p]: neighbor of dart p (sorted within each vertex)
  vector<int> own;    // own[p]: owner vertex of dart p
  vector<int> twin;   // twin[p]: dart p reversed
};

Graph read_graph(InStream& stream) {
  Graph g;
  g.n = stream.readInt();
  g.m = stream.readInt();
  vector<pair<int, int>> edges(g.m);
  g.start.assign(g.n + 1, 0);
  for (auto& [a, b] : edges) {
    a = stream.readInt();
    b = stream.readInt();
    g.start[a + 1]++;
    g.start[b + 1]++;
  }
  for (int v = 0; v < g.n; v++) g.start[v + 1] += g.start[v];
  g.nb.resize(2 * g.m);
  g.own.resize(2 * g.m);
  g.twin.resize(2 * g.m);
  vector<int> pos(g.start.begin(), g.start.end() - 1);
  for (auto [a, b] : edges) {
    int pa = pos[a]++, pb = pos[b]++;
    g.nb[pa] = b;
    g.nb[pb] = a;
    g.own[pa] = a;
    g.own[pb] = b;
    g.twin[pa] = pb;
    g.twin[pb] = pa;
  }
  // sort each adjacency list by neighbor (keeping twins consistent)
  vector<int> order(2 * g.m);
  iota(order.begin(), order.end(), 0);
  for (int v = 0; v < g.n; v++) {
    sort(order.begin() + g.start[v], order.begin() + g.start[v + 1],
         [&](int p, int q) { return g.nb[p] < g.nb[q]; });
  }
  vector<int> where(2 * g.m);
  for (int p = 0; p < 2 * g.m; p++) where[order[p]] = p;
  Graph h = g;
  for (int p = 0; p < 2 * g.m; p++) {
    int q = order[p];
    h.nb[p] = g.nb[q];
    h.own[p] = g.own[q];
    h.twin[p] = where[g.twin[q]];
  }
  return h;
}

// -1: invalid, 0: No, 1: Yes
int read_yes_no(InStream& stream) {
  string YN = stream.readToken();
  if (YN == "No") return 0;
  if (YN == "Yes") return 1;
  return -1;
}

// Reads a rotation system (for each vertex, its neighbors in cyclic order) and
// checks that it is a planar embedding: every connected component with at
// least one edge must satisfy V - E + F = 2, where F is the number of facial
// walks of the rotation system.
void read_embedding(const Graph& g, InStream& stream, int tc) {
  int n = g.n;
  vector<int> nxt(2 * g.m);  // nxt[p]: dart following p in the rotation of own[p]
  vector<char> used(2 * g.m, 0);
  for (int v = 0; v < n; v++) {
    int deg = g.start[v + 1] - g.start[v];
    int first = -1, prev = -1;
    for (int j = 0; j < deg; j++) {
      int w = stream.readInt(0, n - 1, "neighbor");
      int lo = g.start[v], hi = g.start[v + 1];
      int p = int(lower_bound(g.nb.begin() + lo, g.nb.begin() + hi, w) - g.nb.begin());
      if (p == hi || g.nb[p] != w) {
        stream.quitf(_wa, "case %d: vertex %d is not adjacent to vertex %d", tc, w, v);
      }
      if (used[p]) {
        stream.quitf(_wa, "case %d: vertex %d listed twice around vertex %d", tc, w, v);
      }
      used[p] = 1;
      if (prev != -1) nxt[prev] = p;
      else first = p;
      prev = p;
    }
    if (deg > 0) nxt[prev] = first;
  }

  // connected components (over darts / edges)
  vector<int> comp(n, -1);
  int ncomp = 0;
  {
    vector<int> st;
    for (int v = 0; v < n; v++) {
      if (comp[v] != -1 || g.start[v] == g.start[v + 1]) continue;
      comp[v] = ncomp;
      st.push_back(v);
      while (!st.empty()) {
        int u = st.back();
        st.pop_back();
        for (int p = g.start[u]; p < g.start[u + 1]; p++) {
          int w = g.nb[p];
          if (comp[w] == -1) {
            comp[w] = ncomp;
            st.push_back(w);
          }
        }
      }
      ncomp++;
    }
  }
  vector<int> V(ncomp, 0), E(ncomp, 0), F(ncomp, 0);
  for (int v = 0; v < n; v++) {
    if (comp[v] == -1) continue;
    V[comp[v]]++;
    E[comp[v]] += g.start[v + 1] - g.start[v];
  }
  for (int c = 0; c < ncomp; c++) E[c] /= 2;

  // trace faces: the dart after p = (u -> v) on its face is the successor of
  // twin(p) = (v -> u) in the rotation of v
  vector<char> vis(2 * g.m, 0);
  for (int p = 0; p < 2 * g.m; p++) {
    if (vis[p]) continue;
    F[comp[g.own[p]]]++;
    int q = p;
    while (!vis[q]) {
      vis[q] = 1;
      q = nxt[g.twin[q]];
    }
  }
  for (int c = 0; c < ncomp; c++) {
    if (V[c] - E[c] + F[c] != 2) {
      stream.quitf(_wa,
                   "case %d: rotation system is not planar (component with V=%d, E=%d has F=%d)",
                   tc, V[c], E[c], F[c]);
    }
  }
}

int main(int argc, char* argv[]) {
  registerTestlibCmd(argc, argv);

  int t = inf.readInt();
  for (int tc = 0; tc < t; tc++) {
    Graph g = read_graph(inf);
    int ANS = read_yes_no(ans);
    if (ANS == -1) quitf(_fail, "writer's output is invalid (case %d)", tc);
    if (ouf.seekEof()) quitf(_wa, "unexpected EOF in the participant's output (case %d)", tc);
    int OUF = read_yes_no(ouf);
    if (OUF == -1) quitf(_wa, "case %d: expected Yes or No", tc);
    if (ANS == 1) read_embedding(g, ans, tc);
    if (OUF == 1) read_embedding(g, ouf, tc);
    if (ANS == 1 && OUF == 0) quitf(_wa, "case %d: the graph is planar, but participant printed No", tc);
    if (ANS == 0 && OUF == 1) {
      quitf(_fail, "case %d: participant found a planar embedding, but the answer says No", tc);
    }
  }
  if (!ouf.seekEof()) quitf(_wa, "participant's output contains extra tokens");
  quitf(_ok, "%d cases", t);
}
