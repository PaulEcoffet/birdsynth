#include "finch.h"


int main(int argc, char const *argv[]) {
  double alpha[60000] = {0};
  double beta[60000] = {0};
  double out[60000] = {0};
  finch(alpha, beta, 60000, out);
  return 0;
}
