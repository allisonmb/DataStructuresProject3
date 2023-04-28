using namespace std;

#include <string>
#include <vector>

#pragma once

// Takes in a vector in the form of YYYY-MM-DDTHH:MM and return a vector that has the following entries
// out[0] : year
// out[1] : month
// out[2] : day
// out[3] : hour
vector<int> parseTimeString(string timeString){
    vector<int> out(4);
    out[0] = stoi(timeString.substr(0,4));
    out[1] = stoi(timeString.substr(5,2));
    out[2] = stoi(timeString.substr(8,2));
    out[3] = stoi(timeString.substr(11,2));
    return out;
}

struct Node {
    bool is_leaf;
    int num_keys;
    int keys[31]; // ORDER is the maximum number of keys per node
    Node* pointers[31];
    double values[31];
    Node* next; // Only used for leaf nodes
};

class TempBPlusTree {
public:
    TempBPlusTree(vector<string> times, vector<double> temps);
    double getTempFromString(string timeString);
    double getAvgTempFromString(string timeString);
    double getMinTempFromString(string timeString);
    double getMaxTempFromString(string timeString);

private:
    Node* root = new Node();
    int order;  // Equal to 31 unless there are more than 31 years in the data set

    int findYearIndex(int target);
    Node* nodeAtTimeAndDate(int year, int month, int day);
};

// Takes in a target year and finds its corresponding index
// Used to find the index of a certain year in the pointers array of the root node
int TempBPlusTree::findYearIndex(int targetYear){
    for(int LCV = 0; LCV < root -> num_keys; LCV++){
        if(root -> keys[LCV] == targetYear){
            return LCV;
        }
    }
    return -1;
}

Node* TempBPlusTree::nodeAtTimeAndDate(int year, int month, int day){
    int yearIndex = findYearIndex(year);
    if(yearIndex  == -1){
        return nullptr;
    }
    Node* yearNode = root->pointers[yearIndex];

    Node* monthNode = yearNode->pointers[month - 1];
    if (monthNode == nullptr) {
        // No data for this month
        return nullptr;
    }

    Node* dayNode = monthNode->pointers[day - 1];
    if (dayNode == nullptr) {
        // No data for this day
        return nullptr;
    }else{
        return dayNode;
    }

    // No temperature value found for this hour
    return nullptr;
}

double TempBPlusTree::getTempFromString(string timeString){
    vector<int> parsedVector = parseTimeString(timeString);
    Node* dayNode = nodeAtTimeAndDate(parsedVector[0], parsedVector[1], parsedVector[2]);
    if(dayNode -> pointers[parsedVector[3]] != nullptr && dayNode != nullptr){
        return dayNode -> values[parsedVector[3]];
    }else{
        return -1000.0;
    }
}

double TempBPlusTree::getAvgTempFromString(string timeString){
    vector<int> parsedVector = parseTimeString(timeString);
    Node* dayNode = nodeAtTimeAndDate(parsedVector[0], parsedVector[1], parsedVector[2]);
    double avg = 0.0;
    int count = 0;
    if(dayNode != nullptr){
        for(int LCV = 0; LCV < 24; LCV++){
            if(dayNode -> pointers[LCV] != nullptr){
                avg += dayNode -> values[LCV];
                count++;
            }
        }
    }
    avg /= (count * 1.0);
    return avg;
}

double TempBPlusTree::getMinTempFromString(string timeString){
    vector<int> parsedVector = parseTimeString(timeString);
    Node* dayNode = nodeAtTimeAndDate(parsedVector[0], parsedVector[1], parsedVector[2]);
    double min = 1000.0;
    if(dayNode != nullptr){
        for(int LCV = 0; LCV < 24; LCV++){
            if(dayNode -> pointers[LCV] != nullptr){
                if(dayNode -> values[LCV] < min){
                    min = dayNode -> values[LCV];
                }
            }
        }
    }
    return min;
}

double TempBPlusTree::getMaxTempFromString(string timeString){
    vector<int> parsedVector = parseTimeString(timeString);
    Node* dayNode = nodeAtTimeAndDate(parsedVector[0], parsedVector[1], parsedVector[2]);
    double max = -1000.0;
    if(dayNode != nullptr){
        for(int LCV = 0; LCV < 24; LCV++){
            if(dayNode -> pointers[LCV] != nullptr){
                if(dayNode -> values[LCV] > max){
                    max = dayNode -> values[LCV];
                }
            }
        }
    }
    return max;
}

TempBPlusTree::TempBPlusTree(vector<string> times, vector<double> temps){
    // Find the amount of years spanned by the data set
    int startYear = stoi(times[0].substr(0,4));
    int endYear = stoi(times[times.size() - 1].substr(0,4));

    int amtYears = endYear - startYear + 1;

    if(amtYears > 31){
        this -> order = amtYears;
    }else{
        this -> order = 31;
    }


    // Create the Root Node To Store the Years
    root -> is_leaf = false;
    root -> num_keys = amtYears;
    root -> next = nullptr;

    // For each year, create a node to store the months of the year
    for(int LCV = 0; LCV < amtYears; LCV++){
        root -> keys[LCV] = startYear + LCV;
        Node* yearNode = new Node();
        yearNode -> is_leaf = false;
        yearNode -> num_keys = 12;
        yearNode -> next = nullptr;
        root -> pointers[LCV] = yearNode;
        for(int month = 0; month < 12; month++){
            Node* monthNode = new Node();
            monthNode -> is_leaf = false;
            monthNode -> num_keys = 0;
            monthNode -> next = nullptr;
            root -> pointers[LCV] -> pointers[month] = monthNode;

        }
    }

    // Start reading in information from the inputted vectors
    // First we have to start reading in dates, and each time there is a new day, make a new node
    int curYear, curMonth, curDay, curHour;
    double curTemp;
    int prevYear = 0;
    int prevMonth = 0;
    int prevDay = 0;

    // For each entry in the data
    for(int curLine = 0; curLine < times.size(); curLine++){
        vector<int> parsedVector = parseTimeString(times[curLine]);
        string curTimeString = times[curLine];
        curYear = parsedVector[0];
        curMonth = parsedVector[1];
        curDay = parsedVector[2];
        curHour = parsedVector[3];
        curTemp = temps[curLine];

        int yearIndex = findYearIndex(curYear);

        // If the current line starts a new day, add a new key to the current month for the new day
        if(curDay != prevDay){
            Node* dayNode = new Node();
            dayNode -> is_leaf = true;
            dayNode -> num_keys = 1;
            root -> pointers[yearIndex] -> pointers[curMonth - 1] -> pointers[curDay - 1] = dayNode;
            root -> pointers[yearIndex] -> pointers[curMonth - 1] -> num_keys++;
            // If there is a value for a certain hour, the pointer index in that day is a nullptr
            for(int LCV = 0; LCV < 23; LCV++){
                root -> pointers[yearIndex] -> pointers[curMonth - 1] -> pointers[curDay - 1] -> pointers[curHour] = nullptr;
            }

            // Set the next pointers of the day nodes
            if (prevDay > 0) {
                Node* prevDayNode = root->pointers[yearIndex]->pointers[curMonth - 1]->pointers[prevDay - 1];
                if (prevDayNode != nullptr) {
                    prevDayNode->next = dayNode;
                }
            }

            if (curDay < 31 && root->pointers[yearIndex]->pointers[curMonth - 1]->pointers[curDay]) {
                Node* nextDayNode = root->pointers[yearIndex]->pointers[curMonth - 1]->pointers[curDay];
                if (nextDayNode != nullptr) {
                    dayNode->next = nextDayNode;
                }
            }
        }

        // By this point, there has to exist a key for the given day. Lets put the current hour into its place
        root -> pointers[yearIndex] -> pointers[curMonth - 1] -> pointers[curDay - 1] -> keys[curHour] = curHour;
        root -> pointers[yearIndex] -> pointers[curMonth - 1] -> pointers[curDay - 1] -> num_keys++;
        root -> pointers[yearIndex] -> pointers[curMonth - 1] -> pointers[curDay - 1] -> pointers[curHour] = root;
        root -> pointers[yearIndex] -> pointers[curMonth - 1] -> pointers[curDay - 1] -> values[curHour] = curTemp;

        prevYear = curYear;
        prevMonth = curMonth;
        prevDay = curDay;

    }
}