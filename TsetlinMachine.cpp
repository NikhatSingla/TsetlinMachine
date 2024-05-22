#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

//global variables
int maxStates = 400;
int cutOff = maxStates / 2;
int s = 7.5;
int T = 10;
int threshold = 10;
int noOfClasses = 10;
int noOfEpochs = 10;
int clausesPerComponent = 200;
int noOfFeatures = 784;
int noOfLiterals = noOfFeatures * 2;

class Clause
{
    //pending
    //decide if incrementIndex, decrementIndex, s1, s2 should be inline

    //meanings
    //states is a vector of states of each literal (between 1 to maxStates)
    //object is a given input vector of type bool of literals for operations like calculating clause output and giving feedbacks

    private:
    vector<int> states;

    public:
    Clause()
    {
        //initialise states randomly as cutOff or cutOff + 1
        for (int i = 0; i < noOfLiterals; ++i)
        {
            states.push_back(cutOff + ((rand()) & 1));
        }
    }

    //increment and decrement functions
    void incrementIndex(int& i)
    {
        if (states[i] < maxStates)
        {
            ++states[i];
        }
    }

    void decrementIndex(int& i)
    {
        if (states[i] > 1)
        {
            --states[i];
        }
    }

    //clause output
    bool clauseOutput(vector<bool>& object)
    {
        for (int i = 0; i < noOfLiterals; ++i)
        {
            if (states[i] > cutOff)
            {
                if (object[i] == 0)
                {
                    return 0;
                }
            }
        }
        return 1;
    }

    //comparators
    bool s1()
    {
        float random = rand();
        random /= RAND_MAX;
        return (random <= (1.0 / s));
    }

    bool s2()
    {
        float random = rand();
        random /= RAND_MAX;
        return (random <= (1.0 - (1.0 / s)));
    }

    //feedbacks
    void typeIFeedback(vector<bool>& object)
    {
        if (clauseOutput(object)) //if clause output is 1
        {
            for (int i = 0; i < noOfLiterals; ++i)
            {
                if (object[i]) //if literal is 1
                {
                    if (s2()) //if s2() returns "yes"
                    {
                        incrementIndex(i);
                    }
                }
                else //if literal is 0
                {
                    if (s1()) //if s1() returns "yes"
                    {
                        decrementIndex(i);
                    }
                }
            }
        }
        else //if clause output is 0
        {
            for (int i = 0; i < noOfLiterals; ++i)
            {
                if (s1()) //if s1() returns "yes"
                {
                    decrementIndex(i);
                }
            }
        }
    }

    void typeIIFeedback(vector<bool>& object)
    {
        if (clauseOutput(object)) //if clause output is 1
        {
            for (int i = 0; i < noOfLiterals; ++i)
            {
                if (!(object[i])) //if literal is 0
                {
                    if (states[i] <= cutOff)
                    {
                        incrementIndex(i);
                    }
                }
            }
        }
    }
};

class TsetlinMachineComponent
{
    //pending
    //decide whether c1, c2 should be inline or not

    //meanings
    //clauses if a vector of type Claues, holding values of all clauses
    //object is a given input vector of type bool of literals for operations like calculating clause output and giving feedbacks
    
    private:
    vector<Clause> clauses;

    public:
    TsetlinMachineComponent() 
    {
        clauses.resize(clausesPerComponent);
    }

    //class sum
    int classSum(vector<bool>& object)
    {
        int class_sum = 0;
        for (int i = 0; i < clausesPerComponent; ++i)
        {
            if (i & 1) //i is odd, negative polarity clause
            {
                class_sum += (-1 * clauses[i].clauseOutput(object));
            }
            else //i is even, positive polarity clause
            {
                class_sum += clauses[i].clauseOutput(object);
            }
        }
        return class_sum;
    }

    //clip class sum
    float clippedClassSum(vector<bool>& object)
    {
        int class_sum = classSum(object);

        if (class_sum >= T)
        {
            return T;
        }
        else if (class_sum <= -1* T)
        {
            return -1 * T;
        }
        else
        {
            return class_sum;
        }
    }

    //comparators
    bool c1(float& clipped_class_sum)
    {
        float random = rand();
        random /= RAND_MAX;
        return (random <= ((T - clipped_class_sum))/(2 * T));
    }

    bool c2(float& clipped_class_sum)
    {
        float random = rand();
        random /= RAND_MAX;
        return (random <= ((T + clipped_class_sum))/(2 * T));
    }

    //clause trainings
    void expectedClassTraining(vector<bool>& object)
    {
        float clipped_class_sum = clippedClassSum(object);
        for (int i = 0; i < clausesPerComponent; ++i)
        {
            if (c1(clipped_class_sum)) //training to be given
            {
                if (i & 1) //i is odd, clause polarity is negative
                {  
                    clauses[i].typeIIFeedback(object);
                }
                else //i is even, clause polarity is positive
                {
                    clauses[i].typeIFeedback(object);
                }
            }
        }
    }

    void randomClassTraining(vector<bool>& object)
    {
        float clipped_class_sum = clippedClassSum(object);
        for (int i = 0; i < clausesPerComponent; ++i)
        {
            if (c2(clipped_class_sum)) //training to be given
            {
                if (i & 1) //i is odd, clause polarity is negative
                {  
                    clauses[i].typeIFeedback(object);
                }
                else //i is even, clause polarity is positive
                {
                    clauses[i].typeIIFeedback(object);
                }
            }
        }
    }
};

class TsetlinMachine
{
    //meanings
    //machine is a vector of TsetlinMachineComponents for each class, representing our Tsetlin Machine
    //object is a given input vector of type bool of literals for operations like calculating clause output and giving feedbacks
    //label is the known class of object, its value ranges from 0 to noOfClasses - 1
    //data is a vector of objects
    //labels is a vector of int containing label corresponding to each object

    private:
    vector<TsetlinMachineComponent> machine;

    public:
    TsetlinMachine()
    {
        machine.resize(noOfClasses);
    }

    //testing
    bool test(vector<bool>& object, int& label) //for single object, returns whether correctly classified or not
    {
        int maxSum = machine[0].classSum(object);
        int maxSumClass = 0;

        for (int i = 1; i < noOfClasses; ++i)
        {
            int class_sum = machine[i].classSum(object);
            if (class_sum > maxSum)
            {
                maxSum = class_sum;
                maxSumClass = i;
            }
        }

        return (maxSumClass == label);
    }

    float testMachine(vector<vector<bool>>& data, vector<int>& labels) //for all objects in data, returns accuracy of model
    {
        int data_size = data.size();

        long long int correct = 0;
        for (int i = 0; i < data.size(); ++i)
        {
            correct += test(data[i], labels[i]);
        }

        return ((correct * 100.0) / data_size);
    }

    //training
    void train(vector<bool>& object, int& label) //for single object
    {
        int random_class = rand() % noOfClasses;
        while (random_class == label)
        {
            random_class = rand() % noOfClasses;
        }

        machine[label].expectedClassTraining(object);
        machine[random_class].randomClassTraining(object);
    }

    void trainMachine(vector<vector<bool>>& data, vector<int>& labels) //for all objects in data train machine once 
    {
        int data_size  = data.size();

        for (int i = 0; i < data_size; ++i)
        {
            train(data[i], labels[i]);
        }

        cout << "Accuracy: " << testMachine(data, labels);
    }
};

void conversionToBinary(string& temp, vector<vector<bool>>& data, long long int& count, long long int& colCount)
{
    int value = stoi(temp);

    if (value > threshold)
    {
        data[count][colCount++] = 1;
        data[count][colCount++] = 0;
    }
    else
    {
        data[count][colCount++] = 0;
        data[count][colCount++] = 1;
    }
}

int main()
{
    clock_t start = clock(); //initialise the start time to measure program runtime

    srand(time(NULL)); //seed srand with time(NULL)

    //loads training data
    long long int noOfTrainingSamples = 20000;

    ifstream trainingData;
    trainingData.open("/media/virinder/New Volume/Nikhat/TsetlinMachine/sample_data/mnist_train_small.csv");

    vector<vector<bool>> training_data (noOfTrainingSamples, vector<bool>(noOfLiterals));
    vector<int> training_labels (noOfTrainingSamples);

    string temp;
    long long int count = 0;
    long long int colCount;
    while (getline(trainingData, temp, '\n'))
    {
        stringstream s(temp);

        //read first digit from a row and store in labels
        getline(s, temp, ',');
        training_labels[count] = stoi(temp);

        //read subsequent information and store in data as literals based on thresholding
        colCount = 0;
        while (getline(s, temp, ','))
        {
            conversionToBinary(temp, training_data, count, colCount);
        }

        ++count;
    }
    
    //loads testing data
    long long int noOfTestingSamples = 10000;
    ifstream testingData;
    testingData.open("/media/virinder/New Volume/Nikhat/TsetlinMachine/sample_data/mnist_test.csv");

    vector<vector<bool>> testing_data (noOfTestingSamples, vector<bool>(noOfLiterals));
    vector<int> testing_labels (noOfTestingSamples);

    count = 0;
    while (getline(testingData, temp, '\n'))
    {
        stringstream s(temp);

        //read first digit from a row and store in labels
        getline(s, temp, ',');
        testing_labels[count] = stoi(temp);

        //read subsequent information and store in data as literals based on thresholding
        colCount = 0;
        while (getline(s, temp, ','))
        {
            conversionToBinary(temp, testing_data, count, colCount);
        }

        ++count;
    }

    //creates a tsetlin machine to classify MNIST data set
    TsetlinMachine MNIST;

    //train the machine and print accuracy during each epoch
    for (int i = 1; i <= noOfEpochs; ++i)
    {
        cout << "Executing epoch number " << i << ": ";
        MNIST.trainMachine(training_data, training_labels);
        cout << endl;
    }

    //test machine and print accuracy
    cout << "Completed all epochs, now testing the machine on test data." << endl;
    cout << "Accuracy on test data is " << MNIST.testMachine(testing_data, testing_labels);
    cout << endl;

    clock_t end = clock(); //initialise the end time to meausure program runtime

    //calculate time taken to run the program and print to std::cout
    double timeTaken = double(end - start) / CLOCKS_PER_SEC;
    cout << "Time taken (in seconds) is " << timeTaken << endl; 

    //debugging
    //
    // cout << endl;
    // for (auto& i : training_labels)
    // {
    //     cout << i << " ";
    // }
    // return 0;
    //
    // cout << endl;
    // cout << testing_labels[9999];
    // for (int i = 0; i < noOfLiterals; i += 2)
    // {
    //     if (i % 56 == 0)
    //     {
    //         cout << endl;
    //     }
    //     cout << testing_data[9999][i];
    // }
    // cout << endl;
    // cout << training_labels[0];
    // for (int i = 0; i < noOfLiterals; i += 2)
    // {
    //     if (i % 56 == 0)
    //     {
    //         cout << endl;
    //     }
    //     cout << training_data[0][i];
    // }
}