//
//  GeneticAlgorithmComponent.h
//  AI vs Dungeon
//
//  Created by David Parra (davidparraausina@gmail.com) on 29/03/17.
//  Copyright 2017 David Parra. All rights reserved.
//

#include "Components/ActorComponent.h"
#include "Character/NNCharacter.h"
#include "GeneticAlgorithmComponent.generated.h"

#pragma once

struct SGenome
{
    TArray <double> Bits;
    double Fitness;

    SGenome() {};
    SGenome(ANNCharacter *entity);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AI_VS_DUNGEON_API UGeneticAlgorithmComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGeneticAlgorithmComponent();

    void UpdateGenomeFitness(int32 id, float fitness);
    int32 NewGenome(ANNCharacter *character);
    
    void Epoch();

    inline int32 GetMaxPopulationSize() { return mPopulation; }
    inline int32 GetPopulationSize() { return mGenomes.Num(); }
    inline SGenome GetGenome(int32 index) { return mGenomes[index]; }

private:
    SGenome& RouleteWheelSelection();
    void ElitismSelection(int32 amount, TArray <SGenome> &genomes);

    void Mutate(TArray<double> &chromosome);
    void Crossover(const TArray <double> &mom, const TArray <double> &dad, TArray <double> &child1, TArray <double> &child2);

    //The population of genomes
    TArray<SGenome> mGenomes;

    //Size of the population
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    int32 mPopulation = 50;

    //The rate that the chosen chromosomes can swap their bits (to generate a child)
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    float mCrossoverRate = 0.5;

    //The chance that a child chromosome can be mutated (his bits are modified)
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    float mMutationRate = 0.7;

    //The amount to be modified when mutated
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    float mMaxPerturbation = 0.5;

    //How many genomes are selected from elitism
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    int32 mElitismSelection = 2;

    //How many bits per chromosome
    int32 mChromosomeLenght;

    int32 mFittestGenome = 0;
    double mBestFitnessScore = 0.0;
    double mTotalFitnessScore = 0.0;
    int32 mGeneration = 0;
};
