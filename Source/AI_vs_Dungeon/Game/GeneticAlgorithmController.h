//
//  GeneticAlgorithmController.h
//  AI vs Dungeon
//
//  Created by David Parra (davidparraausina@gmail.com) on 29/03/17.
//  Copyright 2017 David Parra. All rights reserved.
//

#pragma once

#include "GameFramework/Actor.h"
#include "GeneticAlgorithmComponent.h"
#include "GeneticAlgorithmController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDeathDelegate, AActor*, newCharacter);

UCLASS()
class AI_VS_DUNGEON_API AGeneticAlgorithmController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGeneticAlgorithmController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    //Called when the previous entity has finished and its fitness is updated
    UFUNCTION(BlueprintCallable, Category = "Controller")
    void SpawnEntity();

    void UpdateEntityFitness(int32 id, double fitness);

	UPROPERTY(BlueprintAssignable, Category = "Character death")
    FCharacterDeathDelegate OnCharacterDeath;

    //Called when the genetic algorithm has solved the problem
    UFUNCTION(BlueprintCallable, Category = "Controller")
    void SetBestGenome();

private:
    UPROPERTY(EditDefaultsOnly, Category = "GA")
    UGeneticAlgorithmComponent* mGAComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    TSubclassOf<class ANNCharacter> mEntity;

    int32 mInitialPopulation;
    int32 mGenomeIndex;

    bool mFoundSolution;
};
