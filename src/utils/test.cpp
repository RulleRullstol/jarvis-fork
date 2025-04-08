#include <iostream>
#include <string>
#include "configHandler.cpp"

using namespace std;

int main() {

    ConfigHandler configHandler;

    // Test getSections
    vector<string> sections;
    sections = configHandler.getSections();
    cout << "Sections: ";
    for (string sec : sections)
        cout << sec << " ";

    cout << endl;

    // Test getValue
    string value = configHandler.getValue("ha_headers", "authorization");
    cout << "Value of section1.key1: " << value << endl;

    // Test getKeyValues
    vector<string> keyValues;
    keyValues = configHandler.getKeyValues("ha_headers");
    cout << "Keys:";
    for (string key : keyValues)
        cout << key << " ";

    cout << endl;
    return 0;
}