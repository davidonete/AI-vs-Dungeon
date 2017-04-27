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

    void Initialize();

    void UpdateGenomeFitness(int32 id, float fitness);
    int32 NewGenome(ANNCharacter *character);
    
    void Epoch();
    void SetBestGenomes(int32 genomeIdx);

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
    UPROPERTY(EditAnywhere, Category = "Configuration")
    int32 mPopulation = 10;

    //The rate that the chosen chromosomes can swap their bits (to generate a child)
    UPROPERTY(EditAnywhere, Category = "Configuration")
    float mCrossoverRate = 0.5f;

    //The chance that a child chromosome can be mutated (his bits are modified)
    UPROPERTY(EditAnywhere, Category = "Configuration")
    float mMutationRate = 0.7f;

    //The amount to be modified when mutated
    UPROPERTY(EditAnywhere, Category = "Configuration")
    float mMaxPerturbation = 0.5f;

    //How many genomes are selected from elitism
    UPROPERTY(EditAnywhere, Category = "Configuration")
    float mElitismSelection = 0.25f;

    //How many bits per chromosome
    int32 mChromosomeLenght;

    int32 mFittestGenome = 0;
    double mBestFitnessScore = 0.0;
    double mTotalFitnessScore = 0.0;
    int32 mGeneration = 0;
};
