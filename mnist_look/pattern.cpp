#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int threshold = 0;
int pixels = 28;

char whiteChar = '_';
char blackChar = ' ';

string read(string filepath)
{
    ifstream readData;
    readData.open(filepath);

    string outputString = "";
    long int counter;

    string temp;
    while(getline(readData, temp, '\n'))
    {
        stringstream s(temp);
        getline(s, temp, ',');
        outputString += temp;
        outputString += '\n';

        counter = 1;
        while (getline(s, temp, ','))
        {
            if (stoi(temp) > threshold)
            {
                outputString += blackChar;
            }
            else
            {
                outputString += whiteChar;
            }

            if (counter % pixels == 0)
            {
                outputString += '\n';
            }

            ++counter;
        }

        outputString += '\n';
    }

    readData.close();

    return outputString;
}

void write(string s, string filepath)
{
    ofstream writeData;
    writeData.open(filepath);

    writeData << s;

    writeData.close();

    return;
}

int main()
{
    write(read("../sample_data/mnist_train_small.csv"), "/media/virinder/New Volume/Nikhat/TsetlinMachine/mnist_look/TrainData.txt");
    write(read("../sample_data/mnist_test.csv"), "/media/virinder/New Volume/Nikhat/TsetlinMachine/mnist_look/TestData.txt");

    return 0;
}