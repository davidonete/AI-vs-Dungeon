
#include "Network.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class TrainingData
{
public:
    TrainingData(const std::string filename);
    inline bool IsEof() { return mTrainingDataFile.eof(); }
    void GetTopology(std::vector<unsigned> &topology);

    unsigned GetNextInputs(std::vector<double> &inputVals);
    unsigned GetTargetOutputs(std::vector<double> &targetOutputsVals);

private:
    std::ifstream mTrainingDataFile;
};

TrainingData::TrainingData(const std::string filename)
{
    mTrainingDataFile.open(filename.c_str());
}

void TrainingData::GetTopology(std::vector<unsigned>& topology)
{
    std::string line;
    std::string label;

    std::getline(mTrainingDataFile, line);
    std::stringstream ss(line);

    ss >> label;
    if (this->IsEof() || label.compare("topology:") != 0)
        abort();

    while (!ss.eof())
    {
        unsigned n;
        ss >> n;
        topology.push_back(n);
    }

}

unsigned TrainingData::GetNextInputs(std::vector<double> &inputVals)
{
    inputVals.clear();

    std::string line;
    std::getline(mTrainingDataFile, line);
    std::stringstream ss(line);

    std::string label;
    ss >> label;
    if (label.compare("in:") == 0)
    {
        double oneValue;
        while (ss >> oneValue)
            inputVals.push_back(oneValue);
    }

    return (unsigned)inputVals.size();
}

unsigned TrainingData::GetTargetOutputs(std::vector<double> &targetOutputsVals)
{
    targetOutputsVals.clear();

    std::string line;
    std::getline(mTrainingDataFile, line);
    std::stringstream ss(line);

    std::string label;
    ss >> label;
    if (label.compare("out:") == 0)
    {
        double oneValue;
        while (ss >> oneValue)
            targetOutputsVals.push_back(oneValue);
    }

    return (unsigned)targetOutputsVals.size();
}

void ShowVectorVals(std::string label, const std::vector<double> &v)
{
    std::cout << label << " ";
    for (unsigned i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";

    std::cout << std::endl;
}

int main()
{
    TrainingData trainData("../data/trainingData.txt");

    std::vector<unsigned> topology;
    trainData.GetTopology(topology);
    Network myNetwork(topology);

    std::vector<double> inputVals, targetVals, resultVals;
    int trainingPass = 0;

    while (!trainData.IsEof())
    {
        trainingPass++;

        std::cout << std::endl << "Pass " << trainingPass;

        //Get new input data and feed it forward:
        if (trainData.GetNextInputs(inputVals) != topology[0])
            break;

        ShowVectorVals(": Inputs:", inputVals);
        myNetwork.FeedForward(inputVals);

        //Collect the network results
        myNetwork.GetResults(resultVals);
        ShowVectorVals("Outputs:", resultVals);

        trainData.GetTargetOutputs(targetVals);
        ShowVectorVals("Targets:", targetVals);

        myNetwork.BackPropagate(targetVals);

        //Report how well the training is working
        std::cout << "Network average error: " << myNetwork.GetRecentAverageError() << std::endl;
    }
    std::cout << std::endl << "Done!";
    
    int a;
    std::cin >> a;

    return 0;
}