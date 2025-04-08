#include <iostream>
#include <string>
#include "configHandler.cpp"
#include "../../libs/cpp-httplib/httplib.h"



int main() {
    httplib::Client client("google.com");
    auto res = client.Get("/");
    if (res) {
        std::cout << "Response status: " << res->status << std::endl;
        std::cout << "Response body: " << res->body << std::endl;
    } else {
        std::cerr << "Error: " << res.error() << std::endl;
    }
}