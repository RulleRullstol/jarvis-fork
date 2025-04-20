#include "audio.h"

string UDPHandler::getBroadcast() {
  ConfigHandler conf;
  return conf.getValue("esp_general", "broadcast_ip");
}
