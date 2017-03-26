#include <iostream>
#include <string>

#include "GeneticAlgorithm.h"

float RandFloat()
{
    return rand() / float(RAND_MAX);
}

float RandFloatClamped()
{
    return (rand() / float(RAND_MAX)) * 2.0f - 1.0f;
}

int RandInt(int min, int max)
{
    return (rand() % (max - min)) + min;
}

void GetNextInputs(unsigned amount, std::vector<double> &inputs)
{
    inputs.clear();

    for (unsigned i = 0; i < amount; i++)
    {
        unsigned random = rand() % 2;
        inputs.push_back((double)random);
    }
}

GA::GA()
{
    CreateStartPopulation();
}

GA::~GA() {}

void GA::Epoch()
{
    UpdateFitnessScore();

    unsigned newChilds = 0;
    std::vector<SGenome> childGenomes;

    //Elitism selection (select the best genomes for the next generation)
    ElitismSelection(mElitismSelection, childGenomes);

    while (childGenomes.size() < mPopulation)
    {
        //Select two parents
        SGenome mom = RouleteWheelSelection();
        SGenome dad = RouleteWheelSelection();

        //Crossover the parents chromosome data
        SGenome child1, child2;
        Crossover(mom.Bits, dad.Bits, child1.Bits, child2.Bits);

        //Operate a mutation chance
        Mutate(child1.Bits);
        Mutate(child2.Bits);

        //Update the NN weights with the new ones obtained from crossover and mutation
        UpdateWeights(child1);
        UpdateWeights(child2);

        //Add the new childs to the new population
        childGenomes.push_back(child1);
        childGenomes.push_back(child2);

        newChilds += 2;
    }
    //Change the old population with the new one
    mGenomes = childGenomes;

    //Increment the generation counter
    mGeneration++;
    std::cout << "\nNew generation: " << mGeneration << std::endl;
}

void GA::Mutate(std::vector<double> &chromosome)
{
    //Traverse the weight vector and mutate each weight dependent on the mutation rate
    for (unsigned i = 0; i < chromosome.size(); i++)
    {
        //do we perturb this chromosome?
        if (RandFloat() < mMutationRate)
        {
            float a = RandFloatClamped();
            //add or subtract a small value to the weight
            chromosome[i] += (RandFloatClamped() * mMaxPerturbation);
        }
    }
}

void GA::Crossover(const std::vector<double> &mom, const std::vector<double> &dad, std::vector<double> &child1, std::vector<double> &child2)
{
    child1.clear();
    child2.clear();

    if ((RandFloat() > mCrossoverRate) || (mom == dad))
    {
        child1 = mom;
        child2 = dad;
        return;
    }

    //Get the first part of the genome
    unsigned crossoverPoint = RandInt(0, mChromosomeLenght - 1);
    for (unsigned i = 0; i < crossoverPoint; i++)
    {
        child1.push_back(mom[i]);
        child2.push_back(dad[i]);
    }

    //Get the second part of the genome
    for (unsigned i = crossoverPoint; i < mom.size(); i++)
    {
        child1.push_back(dad[i]);
        child2.push_back(mom[i]);
    }
}

SGenome& GA::RouleteWheelSelection()
{
    double slice = RandFloat() * mTotalFitnessScore;
    double total = 0;
    unsigned selectedGenome = 0;

    for (unsigned i = 0; i < mPopulation; i++)
    {
        total += mGenomes[i].Fitness;

        if (total > slice)
        {
            selectedGenome = i;
            break;
        }
    }

    return mGenomes[selectedGenome];
}

void GA::ElitismSelection(unsigned amount, std::vector <SGenome> &genomes)
{
    unsigned selectedGenome = 0;

    for (unsigned j = 0; j < amount; j++)
    {
        double bestFit = 0.0;
        bool valid = true;
        for (unsigned i = 0; i < mGenomes.size(); i++)
        {
            if (mGenomes[i].Fitness > bestFit)
            {
                for (unsigned k = 0; k < genomes.size(); k++)
                {
                    if (genomes[k].Fitness == mGenomes[i].Fitness)
                        valid = false; break;
                }

                if (valid)
                {
                    bestFit = mGenomes[i].Fitness;
                    selectedGenome = i;
                }
            }
        }
        genomes.push_back(mGenomes[selectedGenome]);
    }
}

void GA::UpdateFitnessScore()
{
    //Update the total combined fitness of all the genomes (for the roulete wheel selection)
    mTotalFitnessScore = 0;
    mBestFitnessScore = 0;

    //Check the NN performance and update the fitness of each genome
    for (unsigned i = 0; i < mGenomes.size(); i++)
    {
        //Check the NN performance
        mGenomes[i].Fitness = mGenomes[i].NNetwork->GetNetworkPerformance(false);
        mTotalFitnessScore += mGenomes[i].Fitness;

        //std::cout << " Fitness: " << mGenomes[i].Fitness << std::endl;

        if (mGenomes[i].Fitness > mBestFitnessScore)
        {
            mFittestGenome = i;
            mBestFitnessScore = mGenomes[i].Fitness;
            std::cout << "Fitness record: " << mBestFitnessScore << " from genome: " << mFittestGenome << std::endl;;
        }
    }
}

void GA::UpdateWeights(SGenome &genome)
{
    genome.NNetwork->SetConnectionWeights(genome.Bits);
}

void GA::CreateStartPopulation()
{
    for(unsigned i = 0; i < mPopulation; i++)
        mGenomes.push_back(SGenome());

    mChromosomeLenght = (unsigned)mGenomes[0].Bits.size();
}

void GA::TestFittestGenome()
{
    double fitness = mGenomes[mFittestGenome].NNetwork->GetNetworkPerformance(true);
    std::cout << "Total Fitness: " << fitness << std::endl;
}

SGenome::SGenome()
{
    Fitness = 0.0;

    std::vector<unsigned> topology;
    topology.push_back(2);
    topology.push_back(2);
    topology.push_back(1);

    NNetwork = new Network(topology);

    NNetwork->GetConnectionWeights(Bits);
}