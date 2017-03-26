#include <cassert>
#include <iostream>
#include <string>

#include "Network.h"

void ShowVectorVals(std::string label, const std::vector<double> &v)
{
    std::cout << label << " ";
    for (unsigned i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";

    std::cout << std::endl;
}

double GetTargetOutputs(const std::vector<double> &input)
{
    double a = input[0];
    double b = input[1];

    if (a == b) return 0.0;
    return 1.0;
}

Network::Network(const std::vector<unsigned> &topology)
{
    mRecentAverageError = 1.0;
    mRecentAverageSmoothingFactor = 1.0;

    //Create layers
    unsigned numLayers = (unsigned)topology.size();
    for (unsigned layerIdx = 0; layerIdx < numLayers; layerIdx++)
    {
        mLayers.push_back(Layer());

        //if the layer is the last one there are no outputs
        //if not the number of outputs is the number of neurons of the next layer.
        unsigned numOutputs; // = layerIdx == topology.size() - 1 ? 0 : topology[layerIdx + 1];
        if (layerIdx == numLayers - 1)
            numOutputs = 0;
        else
            numOutputs = topology[layerIdx + 1];

        //Fill the layer with neurons (<= we are addin an extra neuron with a constant value (Bias))
        unsigned numNeurons = topology[layerIdx];
        for (unsigned neuronIdx = 0; neuronIdx <= numNeurons; neuronIdx++)
        {
            //Add the new neurons at the start of the array (LIFO)
            mLayers.back().push_back(Neuron(numOutputs, neuronIdx));
        }

        //Force the bias node's output value to 1.0
        mLayers.back().back().SetOutputValue(-1.0);
    }
}

void Network::FeedForward(const std::vector<double>& inputVals)
{
    assert(inputVals.size() == mLayers[0].size() - 1);

    //Set the output values of the first layer(input layer) as the input values they receive
    //Because the input layer does not modify this values at all.
    for(unsigned i = 0; i < inputVals.size(); i++)
        mLayers[0][i].SetOutputValue(inputVals[i]);

    //Forward propagate
    //Go over each neuron of the network excluding the last neuron of each layer (the bias neuron)
    for (unsigned layerIdx = 1; layerIdx < mLayers.size(); layerIdx++)
    {
        Layer &prevLayer = mLayers[layerIdx - 1];
        for (unsigned neuronIdx = 0; neuronIdx < mLayers[layerIdx].size() - 1; neuronIdx++)
            mLayers[layerIdx][neuronIdx].FeedForward(prevLayer);
    }
}

void Network::BackPropagate(const std::vector<double>& targetVals)
{
    //Calculate overall net error (Root mean square error(RMS) of output network errors)
    Layer &outputLayer = mLayers.back();
    mError = 0.0;

    //RMS = sqrt((sum((target value - actual value)^2)) / quantity of values - 1)
    for (unsigned neuronIdx = 0; neuronIdx < outputLayer.size() - 1; neuronIdx++)
    {
        double delta = targetVals[neuronIdx] - outputLayer[neuronIdx].GetOutputValue();
        mError += delta * delta;
    }
    //Get average error squared (not including bias)
    mError /= outputLayer.size() - 1;
    //Get RMS
    mError = sqrt(mError);

    //Recent average measurement
    mRecentAverageError = (mRecentAverageError * mRecentAverageSmoothingFactor + mError) /
                          (mRecentAverageSmoothingFactor + 1.0);

    //Calculate output layer gradients
    for (unsigned neuronIdx = 0; neuronIdx < outputLayer.size() - 1; neuronIdx++)
        outputLayer[neuronIdx].CalculateOutputGradients(targetVals[neuronIdx]);

    //Calculate hidden layers gradients (from back to front)
    for (unsigned layerIdx = (unsigned)mLayers.size() - 2; layerIdx > 0; layerIdx--)
    {
        Layer &hiddenLayer = mLayers[layerIdx];
        Layer &nextLayer = mLayers[layerIdx + 1];

        for (unsigned neuronIdx = 0; neuronIdx < hiddenLayer.size(); neuronIdx++)
            hiddenLayer[neuronIdx].CalculateHiddenGradients(nextLayer);
    }

    //Update connection weights for all layers except the input layer (from back to front)
    for (unsigned layerIdx = (unsigned)mLayers.size() - 1; layerIdx > 0; layerIdx--)
    {
        Layer &layer = mLayers[layerIdx];
        Layer &prevLayer = mLayers[layerIdx -1];

        for (unsigned neuronIdx = 0; neuronIdx < layer.size() - 1; neuronIdx++)
            layer[neuronIdx].UpdateInputWeights(prevLayer);
    }
}

void Network::GetResults(std::vector<double>& resultVals) const
{
    resultVals.clear();

    //Get the output values of the last layer (output layer) and save them on the result vals vector
    for (unsigned neuronIdx = 0; neuronIdx < mLayers.back().size() - 1; neuronIdx++)
        resultVals.push_back(mLayers.back()[neuronIdx].GetOutputValue());
}

void Network::SetConnectionWeights(const std::vector<double> &w)
{
    unsigned connectionIdx = 0;
    //From the fist layer to the last hidden layer (except output layer)
    for (unsigned layerIdx = 0; layerIdx < mLayers.size() - 1; layerIdx++)
    {
        Layer &layer = mLayers[layerIdx];
        //From the first neuron of the layer to the last
        for (unsigned neuronIdx = 0; neuronIdx < layer.size(); neuronIdx++)
        {
            std::vector<Connection> newConnections;
            unsigned numConnections = (unsigned)layer[neuronIdx].GetOutputWeights().size();
            for (unsigned weigthIdx = 0; weigthIdx < numConnections; weigthIdx++)
            {
                newConnections.push_back(Connection(w[connectionIdx]));
                connectionIdx++;
            }
            layer[neuronIdx].SetOutputWeights(newConnections);
        }
    }
}

void Network::GetConnectionWeights(std::vector<double> &w)
{
    w.clear();

    //From the fist layer to the last hidden layer (except output layer)
    for (unsigned layerIdx = 0; layerIdx < mLayers.size() - 1; layerIdx++)
    {
        Layer &layer = mLayers[layerIdx];
        //From the first neuron of the layer to the last
        for (unsigned neuronIdx = 0; neuronIdx < layer.size(); neuronIdx++)
        {
            std::vector<Connection> weights = layer[neuronIdx].GetOutputWeights();
            for (unsigned weigthIdx = 0; weigthIdx < weights.size(); weigthIdx++)
                w.push_back(weights[weigthIdx].Weight);
        }
    }
}

double Network::GetNetworkPerformance(bool debug)
{
    unsigned cases = 4;

    std::vector<double> inputVals, resultVals;
    double outputVal, targetVal, fitness = 0.0;

    for (unsigned i = 0; i < cases; i++)
    {
        inputVals.clear();
        resultVals.clear();

        if (i == 0)
        {
            inputVals.push_back(0.0);
            inputVals.push_back(0.0);
        }
        else if (i == 1)
        {
            inputVals.push_back(0.0);
            inputVals.push_back(1.0);
        }
        else if (i == 2)
        {
            inputVals.push_back(1.0);
            inputVals.push_back(0.0);
        }
        else
        {
            inputVals.push_back(1.0);
            inputVals.push_back(1.0);
        }

        FeedForward(inputVals);
        GetResults(resultVals);

        outputVal = resultVals[0];
        targetVal = GetTargetOutputs(inputVals);

        fitness += 1.0 - abs(targetVal - outputVal);

        if (debug)
        {
            ShowVectorVals("Inputs: ", inputVals);
            ShowVectorVals("Outputs: ", resultVals);
            std::cout << "Target: " << targetVal << " Fitness: " << 1.0 - abs(targetVal - outputVal) << std::endl;
        }
    }

    fitness /= (double)cases;

    return fitness;
}