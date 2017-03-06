
#include "Network.h"
#include <cassert>

//Check
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

//Check
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

//Check
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
