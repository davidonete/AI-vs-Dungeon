
#include "AI_vs_Dungeon.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NNCharacterController.h"

ANNCharacterController::ANNCharacterController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
}

void ANNCharacterController::Possess(APawn *InPawn)
{
    Super::Possess(InPawn);

    Character = Cast<ANNCharacter>(InPawn);

    if (Character && Character->BehaviorTree)
    {
        BlackboardComponent->InitializeBlackboard(*Character->BehaviorTree->BlackboardAsset);
        BehaviorTreeComponent->StartTree(*Character->BehaviorTree);
    }
}

void ANNCharacterController::AgentJump()
{
    if(!Character->GetCharacterMovement()->IsFalling())
        Character->Jump();
}

void ANNCharacterController::AgentMoveLeftRight(bool moveLeft, bool moveRight)
{
    Character->MoveLeftRight(moveLeft, moveRight);
}

void ANNCharacterController::SetNeuralNetworkInputValue(NNInputType type, bool collision)
{
    Character->SetNeuralNetworkInputValue(type, collision);
}

void ANNCharacterController::NeuralNetworkFeedForward()
{
    Character->NeuralNetworkFeedForward();
}

void ANNCharacterController::NeuralNetworkGetOutputValues()
{
    TArray<bool> outputValues = Character->NeuralNetworkGetOutputValues();

    BlackboardComponent->SetValueAsBool("ShouldJump", outputValues[0]);
    BlackboardComponent->SetValueAsBool("ShouldMoveLeft", outputValues[1]);
    BlackboardComponent->SetValueAsBool("ShouldMoveRight", outputValues[2]);
}

void ANNCharacterController::NeuralNetworkBackPropagate()
{
    Character->NeuralNetworkBackPropagate();
}
