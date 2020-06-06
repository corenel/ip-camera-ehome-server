#include "ehome_server.h"

int main() {
  EHomeServer server;
  server.Start();
  sleep(10000);
  server.Stop();
  return 0;
}