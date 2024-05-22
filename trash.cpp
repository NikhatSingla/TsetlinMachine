#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

int main()
{
    // vector<int> myV;
    // srand(time(NULL));

    // float random = rand();
    // random /= RAND_MAX;
    // cout << random;
    // cout << endl <<1.0/7.5;
    // cout << (3 - 0.2)/(2 * 3);

    fstream fin;
    fin.open("/media/virinder/New Volume/Nikhat/TsetlinMachine/sample_data/mnist_train_small.csv");

    string temp;
    long long int count = 0;

    while(getline(fin, temp, '\n'))
    {
        // cout << temp[0] << " ";
        stringstream s(temp);
        getline(s, temp, ',');
        // ++count;
        cout << ++count << " " << temp << endl;
    }
    cout << count;
    
}