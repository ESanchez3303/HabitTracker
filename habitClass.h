#ifndef HABITCLASS_H
#define HABITCLASS_H

#include <string>
#include <vector>
#include <array>
using namespace std;


class habit{
private:
    string name = "";
    string fileName = ""; // <name_name>.txt
    array<bool, 7> week = {0,0,0,0,0,0,0};
    vector<array<bool,7>> history;


public:
    // Constructor
    habit() = default;
    habit(string newName, string newFilePath);

    // Saves the week into the history (NOTE: this should be called periodically)
    void saveWeek();

    // Files Functions:
    bool writeToFile();
    bool makeFromFile();

    // Get Functions
    string getName() { return name; }
    string getFileName() {return fileName; }
    array<bool,7> getWeek() { return week; }
    array<bool,7> getHistory(int targetIndex);


    // Printing Functions
    void printWeek();
    void printHistory(int targetWeek);

};

// =========================================================================================




















#endif // HABITCLASS_H
