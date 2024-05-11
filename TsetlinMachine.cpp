#include <iostream>
using namespace std;

class Clause
{
    private:
    int maxState;
    int noOfFeatures;
    int noOfLiterals;
    float learningRate;
    float forgettingRate;

    public:
    Clause(int noOfFeatures, int maxState, float learningRate)
    {
        this -> noOfFeatures = noOfFeatures;
        this -> maxState = maxState;
        this -> learningRate = learningRate;

        this -> noOfLiterals = 2 * noOfFeatures;
        this -> forgettingRate = 1 - learningRate;

        int states[noOfLiterals] = 
    }

};

class TM
{

};

class multiclassTM
{

};

int main()
{
    return 0;
}