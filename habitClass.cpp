
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;


class habit{
private:
    string name = "";
    string fileName = ""; // <name_name>.txt
    bool week[7];
    vector<bool[7]> history;


public:
    // Constructor
    habit(string newName, string newFilePath);

    // Saves the week into the history (NOTE: this should be called periodically)
    void saveWeek();

    // Files Functions:
    bool writeToFile();
    bool makeFromFile();

    // Get Functions
    string getName() { return name; }
    string getFileName() {return fileName; }

};

// =========================================================================================

// CONSTRUCTOR:
habit::habit(string newName, string newFilePath){
    // Resetting the days in the week to false
    for(auto &day:week)            // For every day in the week
        day = 0;                   // ----> Set 0

    // Fixing the name to be a file name:
    fileName = "";
    for(auto &ch: newName){
        if(ch == ' ')
            fileName+= '_';
        else
            fileName += ch;
    }
    fileName = newFilePath + "/" + fileName + ".txt";
}




void habit::saveWeek(){
    history.push_back(week);    // Pushes into the history the current week
    for(auto &day:week)         // Resets the week to start a new one
        day = 0;
}



// FILES FUNCIONS:
bool habit::writeToFile(){
    /* Writting file format:
     * name_name.txt
     * name:<name>
     * week:1100100
     * <0000000>
     * [...]
     * <0001000>
     */

    // Opening file with <name_name>.txt
    ofstream habitFile(fileName);
    if(!habitFile){
        // ERROR: could not open the file when writting
        return false;
    }

    // Writting name:
    habitFile << "Name:" << name << endl;

    // Writting the current week
    habitFile  << "Week:";
    for(auto &day:week){
        habitFile << (day);
    }
    habitFile << endl;


    // Writting the history that was saved (logic: first in vector is first to print)
    for(auto &weekInstance:history){
        for(auto &day:weekInstance){
            habitFile << day;
        }
        cout << endl;
    }

    habitFile.close();
    return true;
}

bool habit::makeFromFile(){
    // Opening file with <name_name>.txt
    ifstream habitFile(fileName);
    if(!habitFile){
        // ERROR: could not open file when reading
        return false;
    }

    string tempString;
    // Reading to set the name
    getline(habitFile,tempString,':'); // Ignoring the "name:"
    getline(habitFile,name);           // Setting the name


    // Reading to set the week
    getline(habitFile,tempString,':'); // Ignoring the "week:"
    getline(habitFile,tempString);     // Saving the week info


    // Parsing the week
    if((int)tempString.length() != 7){
        cerr << "ERROR: the week string is not len:7, cannot parse. File:" << fileName << ":" << endl;
        habitFile.close();
        return false;
    }

    int dayCounter = 0;
    for(auto &ch:tempString){
        if(ch == '0')
            week[dayCounter] = 0;

        else if(ch == '1')
            week[dayCounter] = 1;
        else{
            cerr << "ERROR: the week string has invalid characters. File:" << fileName << ":" << tempString << endl;
            habitFile.close();
            return false;
        }
        dayCounter++;
    }


    // Reading the history file:
    history.clear();
    while(getline(habitFile,tempString)){
        if((int)tempString.length() != 7){
            cerr << "ERROR: History line is not len=7. File:" << fileName << ":" << tempString << endl;
            habitFile.close();
            return false;
        }

        bool newWeek[7];
        int dayCounter = 0;
        for(auto &ch:tempString){
            if(ch == '0')
                newWeek[dayCounter] = 0;

            else if(ch == '1')
                newWeek[dayCounter] = 1;
            else{
                cerr << "ERROR: history line has invalid characters. File:" << fileName << ":" << tempString << endl;
                habitFile.close();
                return false;
            }
            dayCounter++;
        }

        history.push_back(newWeek);
    }

    habitFile.close();
    return true;
}
















