#include "testlib.h"

using namespace std;

// -1: invalid, 0: No, 1: Yes
int read_ans(InStream& stream) {
  string YN = stream.readToken();
  if (YN == "No") return 0;
  if (YN == "Yes") return 1;
  return -1;
}

int main(int argc, char* argv[]) {
  registerTestlibCmd(argc, argv);

  int ANS = read_ans(ans);
  if (ANS == -1) quitf(_fail, "writer's output is invalid");
  int OUF = read_ans(ouf);
  if (OUF == -1) quitf(_wa, "output must be Yes or No");
  if (ANS != OUF) {
    quitf(_wa, "expected %s, found %s", ANS ? "Yes" : "No", OUF ? "Yes" : "No");
  }
  quitf(_ok, "OK");
}
