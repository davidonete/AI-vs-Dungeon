
#include "Neuron.h"

#include <cmath>

Neuron::Neuron(unsigned numOutputs, unsigned idx)
{
    //Save the neuron index inside the layer for doing previous FeedForward.
    mIdx = idx;

    for (unsigned connection = 0; connection < numOutputs; connection++)
    {
        //Create a connection for each output neuron and set a random weight
        mOutputWeights.push_back(Connection(RandomWeight()));
    }
}

//Check
void Neuron::FeedForward(const Layer &prevLayer)
{
    double sum = 0.0;

    //Sum the previous layer's outputs (which are our inputs)
    //Include the bias neuron from the previous layer
    for (unsigned neuronIdx = 0; neuronIdx < prevLayer.size(); neuronIdx++)
    {
        //F = Sum (neuron input value * neuron weight)
        sum += prevLayer[neuronIdx].GetOutputValue() * 
               prevLayer[neuronIdx].mOutputWeights[mIdx].Weight;
    }

    mOutputVal = Neuron::TransferFunction(sum);
}

void Neuron::CalculateOutputGradients(const double targetValue)
{
    double delta = targetValue - mOutputVal;
    mGradient = delta * Neuron::TransferFunctionDerivative(mOutputVal);
}

void Neuron::CalculateHiddenGradients(const Layer &nextLayer)
{
    double dow = SumDOW(nextLayer);
    mGradient = dow * Neuron::TransferFunctionDerivative(mOutputVal);
}

void Neuron::UpdateInputWeights(Layer &prevLayer)
{
    //Update the connection weights between this neuron and the previous layer
    for (unsigned neuronIdx = 0; neuronIdx < prevLayer.size(); neuronIdx++)
    {
        Neuron &neuron = prevLayer[neuronIdx];
        double oldDeltaWeight = neuron.mOutputWeights[mIdx].DeltaWeight;

        //The output value of the previous neurons magnified by the gradient and the learning rate
        //And the momentum will determine what percentage of the previous delta weight is maintained
        double newDeltaWeight = mLearningRate * neuron.GetOutputValue() * mGradient +
                                mMomentum * oldDeltaWeight;

        neuron.mOutputWeights[mIdx].DeltaWeight = newDeltaWeight;
        neuron.mOutputWeights[mIdx].Weight += newDeltaWeight;
    }
}

double Neuron::TransferFunction(double x)
{
    //Sigmoid = output range [0.0...1.0]
    //...

    //Hyperbolic tangential(tanh) = output range [-1.0...1.0]
    return tanh(x);
}

double Neuron::TransferFunctionDerivative(double x)
{
    //tanh derivative
    return 1.0 - x * x;
}

double Neuron::SumDOW(const Layer & nextLayer) const
{
    double sum = 0.0;

    //Sum our contribution of the errors at the nodes we feed
    for (unsigned neuronIdx = 0; neuronIdx < nextLayer.size() - 1; neuronIdx++)
        sum += mOutputWeights[neuronIdx].Weight * nextLayer[neuronIdx].mGradient;

    return sum;
}

