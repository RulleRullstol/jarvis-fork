#include <iostream>
#include <string>
#include "configHandler.cpp"
#include "../../libs/httplib.h"

int main() {
    std::cout << "Running";
    httplib::Client client("google.com");
    auto res = client.Get("/");
    if (res && res->status)
        std::cout << res->status << std::endl;

}