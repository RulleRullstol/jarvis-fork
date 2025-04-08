#include <iostream>
#include <string>
#include "configHandler.cpp"

using namespace std;

int main() {
    ConfigHandler configHandler;

    // Test getSections
    string sections = configHandler.getSections();
    cout << "Sections: " << sections << endl;

    // Test getValue
    string value = configHandler.getValue("section1", "key1");
    cout << "Value of section1.key1: " << value << endl;

    // Test getKeyValues
    string keyValues = configHandler.getKeyValues("section1");
    cout << "Key-Values in section1: " << keyValues << endl;

    return 0;
}