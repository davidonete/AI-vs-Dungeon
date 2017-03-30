//
//  GeneticAlgorithm.h
//  GANN
//
//  Created by David Parra (davidparraausina@gmail.com) on 22/03/17.
//  Copyright 2017 David Parra. All rights reserved.
//

#include <vector>

#include "Network.h"

struct SGenome
{
    Network *NNetwork = nullptr;
    std::vector <double> Bits;
    double Fitness;

    SGenome();
    //SGenome(const std::vector <double> &w, double f) : Bits(w), Fitness(f) {}

    //friend bool operator < (const SGenome &lhs, const SGenome &rhs) { return (lhs.fitness < rhs.fitness); }
};

class GA
{
public:
    GA();
    ~GA();

    void Epoch();
    void TestFittestGenome();

private:
    SGenome& RouleteWheelSelection();
    void ElitismSelection(unsigned amount, std::vector <SGenome> &genomes);

    void Mutate(std::vector<double> &chromosome);
    void Crossover(const std::vector <double> &mom, const std::vector <double> &dad, std::vector <double> &child1, std::vector <double> &child2);

    void UpdateFitnessScore();
    void UpdateWeights(SGenome &genome);

    void CreateStartPopulation();

    //The population of genomes
    std::vector<SGenome> mGenomes;
    //Size of the population
    unsigned mPopulation = 50;
    //The rate that the chosen chromosomes can swap their bits (to generate a child)
    double mCrossoverRate = 0.5;
    //The chance that a child chromosome can be mutated (his bits are flipped)
    double mMutationRate = 0.7;
    //How many bits per chromosome
    unsigned mChromosomeLenght;
    //How many genomes are selected from elitism
    unsigned mElitismSelection = 2;

    double mMaxPerturbation = 0.5;

    unsigned mFittestGenome = 0;
    double mBestFitnessScore = 0.0;
    double mTotalFitnessScore = 0.0;
    unsigned mGeneration = 0;
};