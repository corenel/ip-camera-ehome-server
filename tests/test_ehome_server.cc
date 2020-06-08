#include "ices/ehome_server.h"

int main() {
  EHomeServer server;
  int camera_index = 0;

  server.Start();
  while (true) {
    sleep(1);
    if (server.IsOnline(camera_index) && server.IsPushingStream(camera_index) &&
        server.IsReceivingFrame(camera_index)) {
      auto frame = server.GetFrame(camera_index);
      break;
    }
  }
  server.Stop();
  return 0;
}