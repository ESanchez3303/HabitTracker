#include "habitClass.h"
#include <iostream>
#include <fstream>
using namespace std;

// CONSTRUCTOR:
habit::habit(string newName, string newFilePath){
    // Resetting the days in the week to false
    for(auto &day:week)            // For every day in the week
        day = 0;                   // ----> Set 0


    name = newName;
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


    // Writting the history that was saved (logic: first in vector, is oldest week, is first to print)
    for(auto &weekInstance:history){
        for(auto &day:weekInstance){
            habitFile << day;
        }
        habitFile << endl;
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

        array<bool,7> newWeek = {0,0,0,0,0,0,0};
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


void habit::printWeek(){
    for(auto &day:week){
        cout << day;
    }
    cout << endl;
}

void habit::printHistory(int targetWeek){
    if(targetWeek == -1){ // print all history
        for(auto &currWeek:history){
            for(auto &day:currWeek){
                cout << day;
            }
            cout << endl;
        }

    }

    if(targetWeek > (history.size()-1)){
        cout << "ERROR: target week is above the size of history." << endl;
        return;
    }


    for(auto &day: history[targetWeek]){
        cout << day;
    }
    cout << endl;

}


array<bool,7> habit::getHistory(int targetWeek){
    if(targetWeek > (history.size()-1) || targetWeek < 0){
        array<bool,7> temp;
        cout << "ERROR: targetweek outside of range" << endl;
        return temp;
    }

    return history[targetWeek];
}





void habit::setDay(int targetDay, bool setTo){
    if(targetDay > 7 || targetDay < 0){
        cerr << "ERROR: in set day, target day is not 0-7: targetDay:" << targetDay << endl;
        return;
    }
    week[targetDay] = setTo;
}








