#include "../obj/lightRequest.h"
#include "configHandler.h"
#include "web/webUtils.h"
#include <array>
#include <iostream>
#include <string>

using namespace std;

void setLights(vector<LightRequest> lrV);
array<bool, 3> validateList(LightRequest &lr, string &l);
