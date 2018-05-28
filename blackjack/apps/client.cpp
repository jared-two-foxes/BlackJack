
#include "ClientApplication.hpp"

int main(int argc, char** argv) {
  ClientApplication app;
  app.setup(argc, argv);
  int r = app.run();
  return r;
}
