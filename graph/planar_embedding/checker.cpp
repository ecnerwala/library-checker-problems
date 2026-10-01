#include <algorithm>
#include <array>
#include <string>
#include <utility>
#include <vector>
#include "testlib.h"

using namespace std;

struct Graph {
  int V, E;
  vector<array<int, 2>> ends;  // ends[e]: endpoints of edge e
  vector<int> start;           // adj[start[v], start[v + 1]) is the adjacency of v
  vector<pair<int, int>> adj;  // (neighbor, edge), sorted by neighbor
};

Graph read_graph(InStream& stream) {
  Graph g;
  g.V = stream.readInt();
  g.E = stream.readInt();
  g.ends.resize(g.E);
  g.start.assign(g.V + 1, 0);
  for (auto& [a, b] : g.ends) {
    a = stream.readInt();
    b = stream.readInt();
    g.start[a + 1]++;
    g.start[b + 1]++;
  }
  for (int v = 0; v < g.V; v++) g.start[v + 1] += g.start[v];
  g.adj.resize(2 * g.E);
  vector<int> pos(g.start.begin(), g.start.end() - 1);
  for (int e = 0; e < g.E; e++) {
    auto [a, b] = g.ends[e];
    g.adj[pos[a]++] = {b, e};
    g.adj[pos[b]++] = {a, e};
  }
  for (int v = 0; v < g.V; v++) {
    sort(g.adj.begin() + g.start[v], g.adj.begin() + g.start[v + 1]);
  }
  return g;
}

// -1: invalid, 0: No, 1: Yes
int read_yes_no(InStream& stream) {
  string token = stream.readToken();
  if (token == "No") return 0;
  if (token == "Yes") return 1;
  return -1;
}

// Reads a rotation system (for each vertex, its neighbors in cyclic order) and
// checks that it is a planar embedding.
// Dart 2 * e + side is edge e directed away from ends[e][side]; nxt[d] is the
// dart after d around its tail, so the dart after d on its face is nxt[d ^ 1].
void read_embedding(const Graph& g, InStream& stream, int tc) {
  int V = g.V, E = g.E;
  vector<int> nxt(2 * E);
  vector<bool> listed(2 * E, false);
  for (int v = 0; v < V; v++) {
    int first = -1, prev = -1;
    for (int i = g.start[v]; i < g.start[v + 1]; i++) {
      int w = stream.readInt(0, V - 1, "neighbor");
      auto lo = g.adj.begin() + g.start[v], hi = g.adj.begin() + g.start[v + 1];
      auto it = lower_bound(lo, hi, pair(w, -1));
      if (it == hi || it->first != w) {
        stream.quitf(_wa, "case %d: vertex %d is not adjacent to vertex %d", tc, w, v);
      }
      int e = it->second;
      int d = 2 * e + (g.ends[e][0] == v ? 0 : 1);
      if (listed[d]) {
        stream.quitf(_wa, "case %d: vertex %d listed twice around vertex %d", tc, w, v);
      }
      listed[d] = true;
      if (prev == -1) first = d;
      else nxt[prev] = d;
      prev = d;
    }
    if (prev != -1) nxt[prev] = first;
  }

  // F = 2C + E - V, over the components with at least one edge
  int expected_face_cycles = 0;
  {
    vector<bool> has_edge(V, false);
    vector<int> par(V, -1);
    auto get_par = [&](int a) -> int {
      while (par[a] >= 0) {
        if (par[par[a]] >= 0) par[a] = par[par[a]];
        a = par[a];
      }
      return a;
    };
    auto merge = [&](int a, int b) -> bool {
      a = get_par(a), b = get_par(b);
      if (a == b) return false;
      if (par[a] > par[b]) swap(a, b);
      par[a] += par[b];
      par[b] = a;
      return true;
    };
    for (int e = 0; e < E; e++) {
      for (int u : g.ends[e]) {
        if (!has_edge[u]) {
          has_edge[u] = true;
          expected_face_cycles++;
        }
      }
      expected_face_cycles += 1 - 2 * merge(g.ends[e][0], g.ends[e][1]);
    }
  }

  int num_face_cycles = 0;
  {
    vector<bool> face_vis(2 * E, false);
    for (int d = 0; d < 2 * E; d++) {
      if (face_vis[d]) continue;
      num_face_cycles++;
      for (int cur = d; !face_vis[cur]; cur = nxt[cur ^ 1]) face_vis[cur] = true;
    }
  }
  // Non-planar rotation systems have fewer faces.
  if (num_face_cycles != expected_face_cycles) {
    stream.quitf(_wa, "case %d: rotation system is not planar (%d faces, a planar embedding has %d)",
                 tc, num_face_cycles, expected_face_cycles);
  }
}

int main(int argc, char* argv[]) {
  registerTestlibCmd(argc, argv);

  int T = inf.readInt();
  for (int tc = 0; tc < T; tc++) {
    Graph g = read_graph(inf);
    int jury_answer = read_yes_no(ans);
    if (jury_answer == -1) quitf(_fail, "writer's output is invalid (case %d)", tc);
    if (ouf.seekEof()) quitf(_wa, "unexpected EOF in the participant's output (case %d)", tc);
    int participant_answer = read_yes_no(ouf);
    if (participant_answer == -1) quitf(_wa, "case %d: expected Yes or No", tc);
    if (jury_answer == 1) read_embedding(g, ans, tc);
    if (participant_answer == 1) read_embedding(g, ouf, tc);
    if (jury_answer == 1 && participant_answer == 0) {
      quitf(_wa, "case %d: the graph is planar, but participant printed No", tc);
    }
    if (jury_answer == 0 && participant_answer == 1) {
      quitf(_fail, "case %d: participant found a planar embedding, but the answer says No", tc);
    }
  }
  if (!ouf.seekEof()) quitf(_wa, "participant's output contains extra tokens");
  quitf(_ok, "%d cases", T);
}
