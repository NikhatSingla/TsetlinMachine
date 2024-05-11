import numpy as np
import random

class Clause:
    def __init__(self, noOfFeatures, learningRate, maxState): #initialises a rule
        self.rand = random.random

        self.maxState = maxState #maxState must be even
        self.cutOff = maxState / 2

        self.noOfFeatures = noOfFeatures
        self.noOfLiterals = noOfFeatures * 2

        self.learningRate = learningRate
        self.forgettingRate = 1 - learningRate

        self.states = np.ones(self.noOfLiterals) * self.cutOff

    def getOutput(self, input): #gives clause output as 0 or 1 depending on input, a numpy array of booleanised value of features
        return np.logical_and.reduce(np.where(self.states > self.cutOff, input, np.ones(self.noOfLiterals)))
    
    def recognizeFeedback(self, input): #input is a numpy array of booleanised value of features
        for i in range(len(self.states)):
            if (input[i] == 0): #needs decrement
                if (self.rand() < self.forgettingRate): #perform the decrement
                    if (self.states[i] >= 2):
                        self.states[i] -= 1
            elif (input[i] == 1): #needs increment
                if (self.rand() < self.learningRate): #perform the increment
                    if (self.states[i] <= self.maxState - 1):
                        self.states[i] += 1

    def eraseFeedback(self, input): #input is a numpy array of booleanised value of features
        for i in range(len(self.states)):
            if (self.rand() < self.forgettingRate):
                if (self.states[i] >= 2):
                    self.states[i] -= 1
    
    def typeIFeedback(self, input):
        if (self.getOutput(input)):
            self.recognizeFeedback(input)
        else:
            self.eraseFeedback(input)

    def typeIIFeedback(self, input):
        for i in range(len(self.states)):
            if ((self.states[i] <= self.cutOff) and (self.states[i] >= 2) and (input[i] == 0)):
                self.states[i] += 1
            
class TM:
    def __init__(self, noOfClauses, noOfFeatures, learningRate, maxState, T):
        self.rand = random.random
        self.noOfClauses = noOfClauses #should be even
        self.T = T

        self.clauses = []
        for i in range(noOfClauses):
            clause = Clause(noOfFeatures, learningRate, maxState)
            self.clauses.append(clause)

    def classSum(self, input):
        sum = 0

        for i in range(self.noOfClauses):
            output = self.clauses[i].getOutput(input)

            if (i % 2 == 0):
                sum += output
            else:
                sum -= output

        return sum
    
    def expectedClassTraining(self, input):
        class_sum = self.classSum(input)
        if (class_sum > self.T):
            class_sum = self.T
        elif (class_sum < -1 * self.T):
            class_sum = -1 * self.T
        
        comparator = (self.T - class_sum) / (2 * self.T)

        for i in range(self.noOfClauses):
            if (self.rand() <= comparator):
                if (i % 2 == 0): #positive polarity clause
                    self.clauses[i].typeIFeedback(input)
                else: #negative polarity clause
                    self.clauses[i].typeIIFeedback(input)

    def randomClassTraining(self, input):
        class_sum = self.classSum(input)
        if (class_sum > self.T):
            class_sum = self.T
        elif (class_sum < -1 * self.T):
            class_sum = -1 * self.T

        comparator = (self.T + class_sum) / (2 * self.T)
        
        for i in range(self.noOfClauses):
            if (self.rand() <= comparator):
                if (i % 2 == 0): #positive polarity clause
                    self.clauses[i].typeIIFeedback(input)
                else: #negative polarity clause
                    self.clauses[i].typeIFeedback(input) 

class Machine:
    def __init__(self, classes, noOfClauses, noOfFeatures, learningRate, maxState, T):
        self.rand = random.randrange
        self.classes = classes
        self.noOfTM = len(classes)

        self.TMs = []
        for i in range(self.noOfTM):
            tm = TM(noOfClauses, noOfFeatures, learningRate, maxState, T)
            self.TMs.append([tm, self.classes[i]])

    def train(self, input, class_):
        for i in range(self.noOfTM):
            if (self.TMs[i][1] == class_):
                expectedTM = self.TMs[i][0]

        randValue = self.rand(0, self.noOfTM)
        while (self.TMs[randValue][1] == class_):
            randValue = self.rand(0, self.noOfTM)
        
        randomTM = self.TMs[randValue][0]

        expectedTM.expectedClassTraining(input)
        randomTM.randomClassTraining(input)

    def classify(self, input):
        maxOutput = 0
        maxOutputIndex = 0

        for i in range(self.noOfTM):
            if (self.TMs[i][0].classSum(input) > maxOutput):
                maxOutput = self.TMs[i][0].classSum(input)
                maxOutputIndex = i
        
        return self.TMs[maxOutputIndex][1]

    def test(self, input, class_):
        if (self.classify(input) == class_):
            return True
        else:
            return False
    
    def testOnData(self, data): #data is an array of the form [[class1, [input1]], [class2, [input2]], ...]
        correct = 0
        incorrect = 0

        for i in data:
            if (self.test(i[1], i[0])):
                correct += 1
            else:
                incorrect += 1
        
        return (correct, incorrect)

    def trainOnData(self, data, epochs): #data is an array of the form [[class1, [input1]], [class2, [input2]], ...]
        for j in range(epochs):
            for i in data:
                self.train(i[1], i[0])
            testResults = self.testOnData(data)
            print(f"Compteted epoch-{j + 1} with accuracy {testResults[0] * 100 / (testResults[0] + testResults[0])}")


class MNIST:
    def __init__(self, epochs, threshold, classes, noOfClauses, noOfFeatures, learningRate, maxState, T):
        self.machine = Machine(classes, noOfClauses, noOfFeatures, learningRate, maxState, T)

        self.epochs = epochs

        with open("sample_data/mnist_train_small.csv") as f:
            from csv import reader
            stream = reader(f)
            data = []
            for i in stream:
                input = np.array([(int(a) > threshold) for a in i[1:]])
                data.append([int(i[0]), (np.array([input,(input^True)]).T).reshape(noOfFeatures*2)])
        self.data = data

        with open("sample_data/mnist_train_small.csv") as f:
            from csv import reader
            stream = reader(f)
            dataTest = []
            for i in stream:
                input = np.array([(int(a) > threshold) for a in i[1:]])
                dataTest.append([int(i[0]), (np.array([input,(input^True)]).T).reshape(noOfFeatures*2)])
        self.testingData = dataTest
    
    def trainAndTest(self):
        self.machine.trainOnData(self.data, self.epochs)
        test = self.machine.testOnData(self.testingData)
        print(f"Success: {(test[0] * 100) / (test[0] + test[1])}, Failure: {(test[1] * 100) / (test[0] + test[1])}")


epochs = 10
threshold = 10
classes = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
noOfClauses = 100
noOfFeatures = 784
learningRate = 0.5
maxState = 400
T = 10

myMNIST = MNIST(epochs, threshold, classes, noOfClauses, noOfFeatures, learningRate, maxState, T)
myMNIST.trainAndTest()