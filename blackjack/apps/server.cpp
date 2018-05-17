
#include "ServerApplication.hpp"

int main(int argc, char* argv[]) {
  Application app;
  app.setup(argc, argv);
  int r = app.run();
  return r;
}
