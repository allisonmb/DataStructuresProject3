#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include "Map.h"
#include "TempBPlusTree.h"

using namespace std;

void readFile(string file1, Map<long long,double> &tempMap, vector<double> &temperatures, vector<string> &times);
void getMinTempMap(string date, Map<long long,double> &tempMap);
void getMaxTempMap(string date, Map<long long,double> &tempMap);
void getAverageTempMap(string date, Map<long long,double> &tempMap);

//For this program, an input file for dates between January 1st 2007 and December 31st 2022 is used
//but a file with any dates can be used as long as the formatting is the same

int main() {

    //Declare map
    Map<long long,double> tempMap = Map<long long,double>();

    //Declare the time and temperature vectors required to input data into the B+ tree
    vector<string> times;
    vector<double> temperatures;

    //Read the data in the file and store that data into the two vectors and the map
    readFile("weather_data.csv", tempMap, temperatures, times);

    //declare the tree
    TempBPlusTree tree = TempBPlusTree(times, temperatures);

    bool yes = true;

    //Menu
    while(yes) {
        cout << "What would you like to do?" << endl;
        cout << "1. Temp @ Date and Time? " << endl;
        cout << "2. Average Temp @ Date? " << endl;
        cout << "3. Highest Temp @ Date? " << endl;
        cout << "4. Lowest Temp @ Date? " << endl;
        cout << "5. Quit" << endl;
        int choice;
        cin >> choice;

        //Return the temperature at the requested date and time
        if (choice == 1) {
            cout << "Enter a Year(xxxx): " << endl;
            string year;
            cin >> year;

            cout << "Enter a Day(xx): " << endl;
            string day;
            cin >> day;

            cout << "Enter a Month(xx): " << endl;
            string month;
            cin >> month;

            cout << "Enter a Time(xx:xx): " << endl;
            string time;
            cin >> time;

            //Get subtrings of the time to remove the colon
            string time1 = time.substr(0,2);
            string time2 = time.substr(3,2);

            //combine the substrings to get the correct formatting to put into the B+ tree and map
            //For the map, all non numerical chars must be removed so the string can be converted to a long long
            //For the B+ tree, the dashes, T, and colon must be included in the string
            //This must be done for all menu options
            string forMap = year + month + day + time1 + time2;
            string forTree = year + "-" + month + "-" + day + "T" + time1 + ":" + time2;

            //Convert the string to a long long and find the corresponding data in the map
            try {
                cout << "Map Result: " << tempMap.find(stoll(forMap)) << " Degrees Celsius" << endl;
            }
            //If the key is not found in the map, output an error
            catch(char const* e){
                cout << "Error: Item Not in Map!" << endl;
            }

           //Find the temperature in the B+ tree and print to console
            cout << "Tree Result: " << tree.getTempFromString(forTree) << " Degrees Celsius" << endl;

            cout << endl;
        }

        //Find the average temperature for the requested date
        else if (choice == 2) {
            cout << "Enter a Year(xxxx): " << endl;
            string year;
            cin >> year;

            cout << "Enter a Day(xx): " << endl;
            string day;
            cin >> day;

            cout << "Enter a Month(xx): " << endl;
            string month;
            cin >> month;

            string date = year + month + day;
            string forTree = year + "-" + month + "-" + day + "T00:00";

            //call the function to get and print the average for the map
            getAverageTempMap(date, tempMap);

            //Get the average value from the values in the tree
            cout << "Tree Result: " << tree.getAvgTempFromString(forTree) << " Degrees Celsius" << endl;

            cout << endl;
        }

        //Find the highest temperature for the given date
        else if (choice == 3) {
            cout << "Enter a Year(xxxx): " << endl;
            string year;
            cin >> year;

            cout << "Enter a Day(xx): " << endl;
            string day;
            cin >> day;

            cout << "Enter a Month(xx): " << endl;
            string month;
            cin >> month;

            string date = year + month + day;
            string forTree = year + "-" + month + "-" + day + "T00:00";

            //Find the maximum temperature from the map and print it to console
            getMaxTempMap(date, tempMap);

            //Find the maximum temperature from the map and print it to console
            cout << "Tree Result: " << tree.getMaxTempFromString(forTree) << " Degrees Celcius" << endl;

            cout << endl;
        }

        //Find the minimum temperature for the given date
        else if (choice == 4) {
            cout << "Enter a Year(xxxx): " << endl;
            string year;
            cin >> year;

            cout << "Enter a Day(xx): " << endl;
            string day;
            cin >> day;

            cout << "Enter a Month(xx): " << endl;
            string month;
            cin >> month;

            string date = year + month + day;
            string forTree = year + "-" + month + "-" + day + "T00:00";

            //Find the minimum temperature for the given date from the tree and print it to the console
            getMinTempMap(date, tempMap);

            //find the minimum temperature for the B+ tree and print it to the console
            cout << "Tree Result: " << tree.getMinTempFromString(forTree) << " Degrees Celcius" << endl;

            cout << endl;
        }

        else if (choice == 5) {
            //set bool to end program
            yes = false;
        }

        //If an invalid menu choice is given then output an error
        else {
            cout << "Not A Valid Response, Try Again!" << endl;
        }
    }
}

//Read in the data from the file to store into the map and the B+ tree
void readFile(string file1, Map<long long,double> &tempMap, vector<double> &temperatures, vector<string> &times){

    // Open the CSV file
    ifstream file(file1);

    // Check if file was successfully opened
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
    }

    // Read the file line by line
    string line;
    int line_number = 1;

    long long tempKey;
    double tempValue;

    string timesString;
    string temperaturesString;

    //getlines get rid of header from csv file
    getline(file, timesString);
    getline(file, timesString);
    getline(file, timesString);
    getline(file, timesString);

    //gets data and reads it in
    while (getline(file, timesString, ',')) {



        times.push_back(timesString);


        //breaking up "timesString" string into numerical substrings
        string str1 = timesString.substr(0, 4);
        //cout << str1 << endl;
        string str2 = timesString.substr(5, 2);
        //cout << str2 << endl;
        string str3 = timesString.substr(8, 2);
        //cout << str3 << endl;
        string str4 = timesString.substr(11, 2);
        //cout << str4 << endl;
        string str5 = timesString.substr(14, 2);
        //cout << str5 << endl;

        //adding all the substrings together into 1 big string, only numerical values!!!
        string tempNum = str1 + str2 + str3 + str4 + str5;

        tempKey = stoll(tempNum);

        //getting rest of the line
        getline(file, temperaturesString, '\n');


        //storing temperaturesString as a double
        double temp = stod(temperaturesString);


        tempValue = temp;


        temperatures.push_back(temp);

        tempMap.insert(tempKey,tempValue);


    }

    // Close the file
    file.close();

}

//Find the minimum temperature for the given date from the map
void getMinTempMap(string date, Map<long long,double> &tempMap){

    vector<double> mapAvg;

    int i;
    int j;

    stringstream ii;
    ii << i;
    string str = ii.str();

    stringstream jj;
    jj << j;
    string str2 = jj.str();


    //Get all temperature values for the given date and store them into a vector for calculations
    for (i = 0; i <= 2; i++)
    {
        for (j = 0; j <= 9; j++)
        {
            string str_i = to_string(i);
            string str_j = to_string(j);

            string temp = date + str_i + str_j + "00";

            //Get each hourly temperature for the date and store it into the vector
            try {
                double data = tempMap.find(stoll(temp));
                mapAvg.push_back(data);
            }
            catch(char const* e){
                cout << "Error: Item Not in Map!" << endl;
            }

            //When the for loop goes past 2 and 3, stop the for loop
            if (i == 2 && j == 3)
            {
                break;
            }
        }

        if (i == 2 && j == 3)
        {
            break;
        }
    }

    //once all values are added to vectors find the minimum
    double minMap = mapAvg[0];
    double minTree = mapAvg[0];
    for(int i = 1; i < mapAvg.size(); i++){
        if(mapAvg[i] < minMap){
            minMap = mapAvg[i];
        }
    }

    //print out the result
    cout << "Map Result: " << minMap << " Degrees Celsius" << endl;
}

//Find the maximum temperature for the given data for the map
void getMaxTempMap(string date, Map<long long,double> &tempMap){

    vector<double> mapAvg;

    int i;
    int j;

    stringstream ii;
    ii << i;
    string str = ii.str();

    stringstream jj;
    jj << j;
    string str2 = jj.str();

    //Get all hourly temperatures for the given date and store into a vector for calculations
    for (i = 0; i <= 2; i++)
    {
        for (j = 0; j <= 9; j++)
        {

            string str_i = to_string(i);
            string str_j = to_string(j);

            //search using string temp
            string temp = date + str_i + str_j + "00";

            //Find the hourly temperatures in the map
            try {
                double data = tempMap.find(stoll(temp));
                mapAvg.push_back(data);
            }
            catch(char const* e){
                cout << "Error: Item Not in Map!" << endl;
            }

            //If the for loop goes past 2 and 3, break out of the for loop
            if (i == 2 && j == 3)
            {
                break;
            }
        }

        if (i == 2 && j == 3)
        {
            break;
        }
    }

    //once all values are added to vectors find the maximum
    double maxMap = 0;
    double maxTree = 0;
    for(int i = 0; i < mapAvg.size(); i++){
        if(mapAvg[i] > maxMap){
            maxMap = mapAvg[i];
        }
    }

    //Print the result
    cout << "Map Result: " << maxMap << " Degrees Celsius" << endl;
}

void getAverageTempMap(const string date, Map<long long,double> &tempMap){


    vector<double> mapAvg;

    int i;
    int j;

    stringstream ii;
    ii << i;
    string str = ii.str();

    stringstream jj;
    jj << j;
    string str2 = jj.str();


    //Go through each hour for the given data and retrieve all hourly temperatures
    for (i = 0; i <= 2; i++)
    {
        for (j = 0; j <= 9; j++)
        {
            //cout << i << j << endl;

            string str_i = to_string(i);
            string str_j = to_string(j);

            //search using string temp
            string temp = date + str_i + str_j + "00";


            //Find the temperature in the map and put it into a vector to be used later for calculations
            try {
                double data = tempMap.find(stoll(temp));
                mapAvg.push_back(data);
            }
            catch(char const* e){
                cout << "Error: Item Not in Map!" << endl;
            }

            //end the for loop when the time values start to go past 23:00
            if (i == 2 && j == 3)
            {
                break;
            }
        }

        if (i == 2 && j == 3)
        {
            break;
        }
    }

    //once all values are added to vectors make computations

    //Calculate the average using the values from the map
    double sumMap;
    double sumTree;
    for(int i = 0; i < mapAvg.size(); i++){
        sumMap += mapAvg[i];
    }
    double mapAvg1 = sumMap/24.0;
    cout << "Map Result: " << mapAvg1 << " Degrees Celsius" <<  endl;
}