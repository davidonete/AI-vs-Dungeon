//
//  Neuron.h
//  GANN
//
//  Created by David Parra (davidparraausina@gmail.com) on 05/03/17.
//  Copyright 2017 David Parra. All rights reserved.
//

#include <vector>
#include <cstdlib>

class Neuron;
typedef std::vector<Neuron> Layer;

struct Connection
{
    double Weight;
    double DeltaWeight;

    Connection(double w) { Weight = w; DeltaWeight = 0.0; }
};

class Neuron
{
public:
    Neuron(const unsigned numOutputs, const unsigned idx);
    ~Neuron() {}

    void FeedForward(const Layer &prevLayer);

    inline void SetOutputValue(const double value) { mOutputVal = value; }
    inline double GetOutputValue() const { return mOutputVal; }
    inline std::vector<Connection> GetOutputWeights() { return mOutputWeights; }
    inline void SetOutputWeights(std::vector<Connection> weights) { mOutputWeights = weights; }

    void CalculateOutputGradients(const double targetValue);
    void CalculateHiddenGradients(const Layer &nextLayer);
    void UpdateInputWeights(Layer &prevLayer);

private:
    static double TransferFunction(const double x);
    static double TransferFunctionDerivative(const double x);
    static double RandomWeight() { return rand() / double(RAND_MAX); }

    double SumDOW(const Layer &nextLayer) const;

    unsigned mIdx;
    double mGradient;
    double mOutputVal;
    std::vector<Connection> mOutputWeights;

    //Overall network learning rate [0.0...1.0]
    double mLearningRate = 0.15;
    //Multiplier of last weight change [0.0...n]
    double mMomentum = 0.5;
};
