// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_vs_Dungeon.h"
#include "Game/AI_vs_DungeonGameInstance.h"
#include "GeneticAlgorithmComponent.h"

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

UGeneticAlgorithmComponent::UGeneticAlgorithmComponent() {}

void UGeneticAlgorithmComponent::Epoch()
{
    int32 newChilds = 0;
    TArray<SGenome> childGenomes;

    //Elitism selection (select the best genomes for the next generation)
    int32 elitismQuantity = (int32)(mPopulation * mElitismSelection);
    ElitismSelection(elitismQuantity, childGenomes);

    while (childGenomes.Num() < mPopulation)
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

        //Add the new childs to the new population
        childGenomes.Add(child1);
        childGenomes.Add(child2);

        newChilds += 2;
    }
    //Change the old population with the new one
    mGenomes = childGenomes;

    //Increment the generation counter
    mGeneration++;
    UE_LOG(LogTemp, Warning, TEXT("New Generation: %d"), mGeneration);

    UAI_vs_DungeonGameInstance *gameInstance = Cast<UAI_vs_DungeonGameInstance>(GetWorld()->GetGameInstance());
    if (gameInstance)
        gameInstance->SetGenerations(mGeneration);
}

void UGeneticAlgorithmComponent::SetBestGenomes(int32 genomeIdx)
{
    int32 newChilds = 0;
    TArray<SGenome> childGenomes;
    
    SGenome bestGenome = mGenomes[genomeIdx];
    while (childGenomes.Num() < mPopulation)
    {
        childGenomes.Add(bestGenome);
        newChilds++;
    }

    //Change the old population with the new one
    mGenomes = childGenomes;
}

void UGeneticAlgorithmComponent::Mutate(TArray<double> &chromosome)
{
    //Traverse the weight vector and mutate each weight dependent on the mutation rate
    for (int32 i = 0; i < chromosome.Num(); i++)
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

void UGeneticAlgorithmComponent::Crossover(const TArray<double> &mom, const TArray<double> &dad, TArray<double> &child1, TArray<double> &child2)
{
    child1.Empty();
    child2.Empty();

    if ((RandFloat() > mCrossoverRate) || (mom == dad) || mCrossoverRate <= 0.0f)
    {
        child1 = mom;
        child2 = dad;
        return;
    }

    //Get the first part of the genome
    int32 crossoverPoint = RandInt(0, mChromosomeLenght - 1);
    for (int32 i = 0; i < crossoverPoint; i++)
    {
        child1.Add(mom[i]);
        child2.Add(dad[i]);
    }

    //Get the second part of the genome
    for (int32 i = crossoverPoint; i < mom.Num(); i++)
    {
        child1.Add(dad[i]);
        child2.Add(mom[i]);
    }
}

SGenome& UGeneticAlgorithmComponent::RouleteWheelSelection()
{
    double slice = RandFloat() * mTotalFitnessScore;
    double total = 0;
    int32 selectedGenome = 0;

    for (int32 i = 0; i < mPopulation; i++)
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

void UGeneticAlgorithmComponent::ElitismSelection(int32 amount, TArray <SGenome> &genomes)
{
    int32 selectedGenome = 0;

    for (int32 j = 0; j < amount; j++)
    {
        double bestFit = 0.0;
        bool valid = true;
        for (int32 i = 0; i < mGenomes.Num(); i++)
        {
            if (mGenomes[i].Fitness > bestFit)
            {
                for (int32 k = 0; k < genomes.Num(); k++)
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
        genomes.Add(mGenomes[selectedGenome]);
    }
}

/*
void UGeneticAlgorithmComponent::UpdateFitnessScore()
{
    //Update the total combined fitness of all the genomes (for the roulete wheel selection)
    mTotalFitnessScore = 0;
    mBestFitnessScore = 0;

    //Check the NN performance and update the fitness of each genome
    for (int32 i = 0; i < mGenomes.Num(); i++)
    {
        //Check the NN performance
        //mGenomes[i].Fitness = mGenomes[i].NNetwork->GetNetworkPerformance(false);
        mTotalFitnessScore += mGenomes[i].Fitness;

        //std::cout << " Fitness: " << mGenomes[i].Fitness << std::endl;

        if (mGenomes[i].Fitness > mBestFitnessScore)
        {
            mFittestGenome = i;
            mBestFitnessScore = mGenomes[i].Fitness;
            //std::cout << "Fitness record: " << mBestFitnessScore << " from genome: " << mFittestGenome << std::endl;;
        }
    }
}

void UGeneticAlgorithmComponent::UpdateWeights(SGenome &genome)
{
    //genome.NNetwork->SetConnectionWeights(genome.Bits);
}
*/

void UGeneticAlgorithmComponent::Initialize()
{
    UAI_vs_DungeonGameInstance *gameInstance = Cast<UAI_vs_DungeonGameInstance>(GetWorld()->GetGameInstance());
    if (gameInstance && gameInstance->GetInitialConfigValues())
    {
        mPopulation = gameInstance->GetPopulationSize();
        mCrossoverRate = (float)gameInstance->GetCrossoverRate() * 0.01f;
        mMutationRate = (float)gameInstance->GetMutationRate() * 0.01f;
        mMaxPerturbation = (float)gameInstance->GetMaxPerturbation() * 0.01f;
        mElitismSelection = (float)gameInstance->GetElitismRate() * 0.01f;
    }
}

void UGeneticAlgorithmComponent::UpdateGenomeFitness(int32 id, float fitness)
{
    mGenomes[id].Fitness = fitness;

    if (fitness > mBestFitnessScore)
    {
        //Update fitness on GUI
        UAI_vs_DungeonGameInstance *gameInstance = Cast<UAI_vs_DungeonGameInstance>(GetWorld()->GetGameInstance());
        if (gameInstance && gameInstance->GetBestFitness() < fitness)
            gameInstance->SetBestFitness(fitness);
    }
}

int32 UGeneticAlgorithmComponent::NewGenome(ANNCharacter *character)
{
    mGenomes.Add(SGenome(character));
    mChromosomeLenght = (int32)mGenomes[0].Bits.Num();
    return mGenomes.Num() - 1;
}

SGenome::SGenome(ANNCharacter *entity)
{
    Fitness = 0.0;
    entity->NeuralNetworkGetConnectionWeights(Bits);
}