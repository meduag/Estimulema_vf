
#include <vector>

struct data {
  int uno;
  int dos;
};

struct data * numV1[1];

void setup() {
numV1[0] = (struct data*) malloc(2 * sizeof(struct data));
}

void loop() {

}
