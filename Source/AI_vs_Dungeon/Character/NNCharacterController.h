//
//  NNCharacterController.h
//  AI vs Dungeon
//
//  Created by David Parra (davidparraausina@gmail.com) on 12/03/17.
//  Copyright � 2016 David Parra. All rights reserved.
//

#pragma once

#include "AIController.h"
#include "NNCharacter.h"
#include "NNCharacterController.generated.h"

UCLASS()
class AI_VS_DUNGEON_API ANNCharacterController : public AAIController
{
    GENERATED_BODY()

    UPROPERTY(transient)
    UBlackboardComponent *BlackboardComponent;

    UPROPERTY(transient)
    UBehaviorTreeComponent *BehaviorTreeComponent;
	
public:
    ANNCharacterController();

    virtual void Possess(APawn *InPawn) override;

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void AgentJump();

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void AgentMoveLeftRight(bool moveLeft, bool moveRight);

    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    void SetNeuralNetworkInputValue(NNInputType type, bool collision);

    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    void NeuralNetworkFeedForward();

    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    void NeuralNetworkGetOutputValues();

    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    void NeuralNetworkBackPropagate();

private:
    ANNCharacter* Character;

};
