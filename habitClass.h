#ifndef HABITCLASS_H
#define HABITCLASS_H

#include <string>
#include <vector>
#include <array>
#include <QDate>
using namespace std;

struct historyWeek{
    array<bool,7> values;
    QDate start;
    QDate end;
};


class habit{
private:
    string name = "";
    string fileName = ""; // <name_name>.txt
    array<bool, 7> week = {0,0,0,0,0,0,0};

    vector<historyWeek> history;              // {z,x,x,x,x,y} | 'y' is newer information while 'z' is oldest information


public:
    // Constructor
    habit() = default;
    habit(string newName, string newFilePath);

    // Variables
    QString dateFormat = "MM/dd/yyyy";

    // Saves the week into the history (NOTE: this should be called when date goes from SUN to MON)
    void saveWeek(QDate today);

    // Files Functions:
    bool writeToFile();
    string makeFromFile();

    // Get Functions
    string getName() { return name; }
    string getFileName() {return fileName; }
    array<bool,7> getWeek() { return week; }
    historyWeek getLastWeek();
    int getHistorySize(){return history.size();}
    vector<historyWeek>& getHistory(){return history;}


    // Printing Functions
    void printWeek();


    // Set Functions:
    void setDay(int targetDay, bool setTo);

};



#endif // HABITCLASS_H
