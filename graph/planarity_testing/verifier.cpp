#include <algorithm>
#include <utility>
#include <vector>
#include "testlib.h"
#include "params.h"

using namespace std;

int main(int argc, char* argv[]) {
  registerValidation(argc, argv);

  int n = inf.readInt(N_MIN, N_MAX, "N");
  inf.readSpace();
  int m = inf.readInt(M_MIN, M_MAX, "M");
  inf.readChar('\n');

  vector<pair<int, int>> edges(m);
  for (int i = 0; i < m; i++) {
    int a = inf.readInt(0, n - 1, "a_i");
    inf.readSpace();
    int b = inf.readInt(0, n - 1, "b_i");
    inf.readChar('\n');
    ensuref(a != b, "self-loop at vertex %d (edge %d)", a, i);
    edges[i] = {min(a, b), max(a, b)};
  }
  inf.readEof();

  sort(edges.begin(), edges.end());
  for (int i = 1; i < m; i++) {
    ensuref(edges[i - 1] != edges[i], "duplicate edge between %d and %d",
            edges[i].first, edges[i].second);
  }
  return 0;
}
