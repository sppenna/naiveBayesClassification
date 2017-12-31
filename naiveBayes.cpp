/*
This program takes two training files of Shakespearean and Elizabethan sonnets.

Using Naive Bayes the program will read a set of test sonnets and predict whether the sonnet is Shakespearean or not.
The prediction will then be compared with the actual answer to determine how accurate the prediction is.
*/

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>

using namespace std;

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

int checkWord(string word, const vector<string>& sonnet)
{
    //Linear search to check if word exists in the array sonnet
    for (int i=0; i<sonnet.size(); ++i)
    {
        if (!sonnet[i].compare(word))
            return 1;
    }
    
    return 0;
    
}

void populateFeature(const vector<string>& sonnet, const vector<string>& dictionary, vector<float>& currentFeature)
{
    
    //Go through each word in dictinary and check if it exists in sonnet
    for (int i=0; i<dictionary.size(); ++i)
    {
        currentFeature[i] = checkWord(dictionary[i], sonnet);
    }
    
}

int main(int argc, const char * argv[]) {

    //Enter the names of the Shakespearean training file, Elizabethan training file, Shakespearean test file and Elizabethan test file (in that order):
    cout << "Enter the names of the Shakespearean training file, Elizabethan training file, Shakespearean test file and Elizabethan test file (in that order): " <<
        endl;
    int isShake = 0, isEliza = 0;
    
    //open the dictionary file.  
    ifstream dictFile;
    dictFile.open("dictionary.txt");
    
    //save the lines of the dictionary into a string.
    stringstream buffer;
    buffer << dictFile.rdbuf();
    string dictionary = buffer.str();
    
    //store each word from the dictionary string into a vector, excluding new line character.
    vector<string> dictWords = split(dictionary, '\n'); 

    //Define the Shakespeare training file. Takes the users input for the file name and opens the corresponding file.
    ifstream shakeFile;
    string input1;
    cin >> input1;
    shakeFile.open(input1.c_str());
    
    //Define the Elizabethan training file. Takes the users input for the file name and opens the corresponding file.
    ifstream elizaFile;
    string input2;
    cin >> input2;
    elizaFile.open(input2.c_str());
    
    //defines a string for each sonnet.
    string sonnet;

    //creates the data file which is a 2D vector of doubles.
    vector<vector<float> > data;
    
    //The length of each line in the data file should be the number of words in the dictionary vectore (354)
    int numWords = dictWords.size();
    
    //reads through the Shakespearean training file line by line.
    while (getline(shakeFile, sonnet)) {
        //each line the Shakespeare counter is incremented and the line is turned into a vector of doubles corresponding to which
        //dictionary word is present in the line.
        isShake++;
        istringstream iss(sonnet);

        string temp = iss.str();

        vector<string> sonnetWords = split(temp, ' ');
        vector<float> currentFeature(numWords);
        
        //compares the sonnet string to the dictionary and returns a vector of 1's and 0's
        populateFeature(sonnetWords, dictWords, currentFeature);
        
        //pushes the vector of 1's and 0's into the 2D-vector of data
        data.push_back(currentFeature);        
    }

    //same as the above while loop except the file is reading from the Elizabethan training file.
    while (getline(elizaFile, sonnet)) {
        isEliza++;
        istringstream iss(sonnet);

        string temp = iss.str();

        vector<string> sonnetWords = split(temp, ' ');
        vector<float> currentFeature(numWords);

        populateFeature(sonnetWords, dictWords, currentFeature);

        data.push_back(currentFeature);
    }
    
    //declares two new strings for the Shakespearean and Elizabethan test files.
    ifstream shakeTest;
    string input3;
    cin >> input3;
    shakeTest.open(input3.c_str());

    ifstream elizaTest;
    string input4;
    cin >> input4;
    elizaTest.open(input4.c_str());
    
    vector<vector<float> > test;
    float shakeTesting = 0, elizaTesting;

    //read throught the Shakespearean test file and saves the corresponding feature vector into a 2D-vector of tests
    while (getline(shakeTest, sonnet)) {
        shakeTesting++;
        istringstream iss(sonnet);

        string temp = iss.str();

        vector<string> sonnetWords = split(temp, ' ');
        vector<float> currentFeature(numWords);

        populateFeature(sonnetWords, dictWords, currentFeature);

        test.push_back(currentFeature);
    }
    
    //Same as the above while loop except it's reading from the Elizabethan testing file.
    while (getline(elizaTest, sonnet)) {
        elizaTesting++;
        istringstream iss(sonnet);

        string temp = iss.str();
        
        vector<string> sonnetWords = split(temp, ' ');
        vector<float> currentFeature(numWords);

        populateFeature(sonnetWords, dictWords, currentFeature);

        test.push_back(currentFeature);

    }
    //Two vectors for probabilities is Shakespeare and is Elizabethan.
    vector<float> probS;
    vector<float> probI;
    float temp = 0;

    float totalS = isShake+isEliza;
    
    //store Shakespearean probabilities
    for (int i = 0; i < numWords; i++) {
        float counterS = 0;
        for (int j = 0; j < isShake; j++) {
            if (data[j][i] > 0) {
                counterS++;
            }
        }
        temp = (counterS+1)/(isShake+2);
        probS.push_back(temp);
    }

    //store Elizabethan probabilities
    for (int i = 0; i < numWords; i++) {
        float counterI = 0;
        for (int j = isShake; j < (isShake+isEliza); j++) {
            if (data[j][i] < 1) {
                counterI++;
            }   
        }
        temp = (counterI+1)/(isEliza+2);
        probI.push_back(temp);
    }

    //reads through the 2D-testing vector
    float prob;
    float shakeCounter = 0, elizaCounter = 0;
    for (int i = 0; i < (shakeTesting+elizaTesting); i++) {
        float PS = log(isShake/(totalS));
        float PI = log((isEliza)/(totalS));
        float testS = 1;
        float testI = 1;
        for (int j = 0; j < numWords; j++) {
            if (test[i][j] > 0) {               
                testS += log(probS.at(j));           //if this position in the 2D testing vector is 1 multiply testS by the corresponding probability in probS
                testI += log(1-probI.at(j));         //and testI by 1-probI.
            }
            if (test[i][j] < 1) {
                testS += log(1-probS.at(j));         //if this position in the 2D testing vector is 0 multiply testS by 1-probS
                testI += log(probI.at(j));           //and testI by probI
            }
            
        }
        PS = PS + testS;
        PI = PI + testI;
        if (PS > PI && i <= shakeTesting) {
            shakeCounter++;
        } 
        if (PI > PS && i >= shakeTesting) {
            elizaCounter++;   
        }
    }
    

    cout << "total number of words in dictionary: " << numWords << endl;
    cout << "total sonnet in " << input1 << " are: " << isShake << endl;
    cout << "total sonnet in " << input2 << " are: " << isEliza << endl;

    cout << "\n" << "total sonnet in " << input3 << " " << shakeTesting << endl;
    cout << "Classification accuracy (" << shakeCounter << "/" << shakeTesting << "): " << (shakeCounter/shakeTesting)*100 << endl;
    cout << "total sonnet in " << input4 << " " << elizaTesting << endl;
    cout << "Classification accuracy (" << elizaCounter << "/" << elizaTesting << "): " << (elizaCounter/elizaTesting)*100 << endl;
    cout << "\n" << "Combined Classification accuracy (" << (elizaCounter+shakeCounter) << "/" << (shakeTesting+elizaTesting) << "): " 
        << (elizaCounter+shakeCounter)/(shakeTesting+elizaTesting)*100 << endl;

    return 0;
}
