#include "habitClass.h"
#include <iostream>
#include <fstream>
#include <QMessageBox>

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




void habit::saveWeek(QDate today){

    cout << "History BEFORE: " << endl;
    for(auto &entry : history){
        cout << entry.start.toString(dateFormat).toStdString() << " - ";
        cout << entry.end.toString(dateFormat).toStdString() << " | ";
        for(auto &value : entry.values)
            cout << value;
        cout << endl;
    }

    // Making new historyWeek object
    historyWeek newEntry;

    // Find the week start date and end date and SET into the history week
    newEntry.start = today.addDays(-today.dayOfWeek() - 6);
    newEntry.end = newEntry.start.addDays(6);

    // Load the values of current week into the historyWeek
    newEntry.values = week;

    // Push the week into the history
    history.push_back(newEntry);

    // Reset the week to all "false"
    for(auto &value : week){
        value = 0;
    }

    cout << "History AFTER: " << endl;
    for(auto &entry : history){
        cout << entry.start.toString(dateFormat).toStdString() << " - ";
        cout << entry.end.toString(dateFormat).toStdString() << " | ";
        for(auto &value : entry.values)
            cout << value;
        cout << endl;
    }

    // Leave the writting for outside of this function by calling the writeToFile() function
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
    for(auto& entry:history){
        habitFile << "[" << entry.start.toString(dateFormat).toStdString() << ":";
        habitFile << entry.end.toString(dateFormat).toStdString() << "]";
        for(auto& value:entry.values){
            habitFile << value;
        }
        habitFile << endl;
    }

    habitFile.close();
    return true;
    return true;
}

string habit::makeFromFile(){
    // Opening file with <name_name>.txt
    ifstream habitFile(fileName);
    if(!habitFile){
        // ERROR: could not open file when reading, just return false
        return "Could not open the file:" + fileName;
    }

    string tempString;

    name = "";

    try{
    // Reading to set the name
    getline(habitFile,tempString,':'); // Ignoring the "name:"
    getline(habitFile,name);           // Setting the name


    getline(habitFile,tempString,':'); // Ignoring the "week:"
    }
    catch(...){
        habitFile.close();
        return "Could not read correctly the name/week labels. File:" + fileName;
    }

    if(name.empty()){
        habitFile.close();
        return "Could not set name correctly. File:" + fileName;
    }

    // Parsing the week
    getline(habitFile,tempString);     // Saving the week info
    if((int)tempString.length() != 7){
        habitFile.close();
        return "The current week string len. is not 7. File:" + fileName;
    }

    int dayCounter = 0;
    for(auto &ch:tempString){
        if(ch == '0')
            week[dayCounter] = 0;

        else if(ch == '1')
            week[dayCounter] = 1;
        else{
            habitFile.close();
            return "The current week has invalid character. File:" + fileName;
        }
        dayCounter++;
    }


    // Reading the history section:
    history.clear();
    while(getline(habitFile,tempString)){
        if(tempString.empty()) // only the last line should be empty
            continue;

        // Getting date from the history entry
        string start = tempString.substr(1, 10);
        string end = tempString.substr(12, 10);
        string data  = tempString.substr(tempString.find(']')+1);

        if((int)data.length() != 7){
            habitFile.close();
            return "History line len. is not 7. File:" + fileName + " | Line:" + data;
        }

        // Parsing through the data to set newWeek entry
        array<bool,7> newWeek = {0,0,0,0,0,0,0};
        int dayCounter = 0;
        for(auto &ch:data){
            if(ch == '0')
                newWeek[dayCounter] = 0;

            else if(ch == '1')
                newWeek[dayCounter] = 1;
            else{
                habitFile.close();
                return "History line has invalid character. File:" + fileName + " | Line:" + tempString;
            }
            dayCounter++;
        }
        historyWeek newHistWeek;
        newHistWeek.start = QDate::fromString(start.c_str(),dateFormat);
        newHistWeek.end = QDate::fromString(end.c_str(),dateFormat);
        newHistWeek.values = newWeek;

        if (!newHistWeek.start.isValid() || !newHistWeek.end.isValid()) {
            habitFile.close();
            return "History line has invalid dates. File:" + fileName + " | Start:" + start + ", End:" + end;
        }

        if(newHistWeek.start.addDays(6) != newHistWeek.end){
            habitFile.close();
            return "History line is not 7 days. File:" + fileName + " | START:" + newHistWeek.start.toString(dateFormat).toStdString()
                   + ", END:" + newHistWeek.end.toString(dateFormat).toStdString();
        }

        if(history.size() != 0){ // If there are currently some entries in the history
            if(newHistWeek.start <= history[history.size()-1].end){ // If this new entry start is BEFORE the last entry end
                habitFile.close();
                return "History lines are not in order. START:" + newHistWeek.start.toString(dateFormat).toStdString() +
                       " is BEFORE/EQUAL TO the previous END:" + history[history.size()-1].end.toString(dateFormat).toStdString();
            }

        }

        history.push_back(newHistWeek);
    }

    habitFile.close();

    return "";
}


void habit::printWeek(){
    for(auto &day:week){
        cout << day;
    }
    cout << endl;
}









void habit::setDay(int targetDay, bool setTo){
    if(targetDay > 7 || targetDay < 0){
        cerr << "ERROR: in set day, target day is not 0-7: targetDay:" << targetDay << endl;
        return;
    }
    week[targetDay] = setTo;
}


historyWeek habit::getLastWeek(){
    return history[history.size()-1];
}
















