
#include "ServerApplication.hpp"

int main(int argc, char* argv[]) {
  Application app;
  app.setup(argc, argv);
  app.run();
  return 0;
}
