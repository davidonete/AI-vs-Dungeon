//
//  Network.h
//  NeuralNetwork
//
//  Created by David Parra (davidparraausina@gmail.com) on 05/03/17.
//  Copyright � 2016 David Parra. All rights reserved.
//


#include <vector>

#include "Neuron.h"

typedef std::vector<Neuron> Layer;

class Network
{
public:
    Network(const std::vector<unsigned> &topology);
    ~Network() {}

    void FeedForward(const std::vector<double> &inputVals);
    void BackPropagate(const std::vector<double> &targetVals);
    void GetResults(std::vector<double> &resultVals) const;
    inline double GetRecentAverageError() const { return mRecentAverageError; }

private:
    //mLayers[layer index][neuron index]
    std::vector<Layer> mLayers;

    double mError;
    double mRecentAverageError;
    double mRecentAverageSmoothingFactor;
};
