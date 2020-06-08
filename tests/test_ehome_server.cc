#include <string>

#include "ices/ehome_server.h"

int main() {
  std::string public_ip = "192.168.199.5";
  EHomeServer server(public_ip);
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