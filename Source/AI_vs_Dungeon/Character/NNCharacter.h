//
//  NNCharacter.h
//  AI vs Dungeon
//
//  Created by David Parra (davidparraausina@gmail.com) on 12/03/17.
//  Copyright � 2016 David Parra. All rights reserved.
//

#pragma once
#include "GameFramework/Character.h"
#include "NeuralNetworkComponent.h"
#include "NNCharacter.generated.h"

UCLASS()
class ANNCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

private:
    /** Called for side to side input */
    void MoveRight(float Val);

    void BeginPlay() override;
    void Tick(float DeltaTime) override;

    //Check if the agent is stuck on the same place for a amount of time
    void CheckCharacterFitness(float DeltaTime);

    float mLastDistanceLeft = 9999999.0f;
    float mLastDistanceUpdateTime = 0.0f;

    float mLastMovementValue = 0.0f;

    FVector mGoalLocation;
    FVector mInitialLocation;
    FRotator mInitialRotation;

    TArray<TArray<double>> mInputCache;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    UNeuralNetworkComponent* NeuralNetworkComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Behavior")
    class UBehaviorTree* mBehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "Behavior")
    TSubclassOf<class ACharacter> mCharacterBody;

public:
    ANNCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    UFUNCTION(BlueprintCallable, Category = "Death")
    void Die();

    UFUNCTION(BlueprintCallable, Category = "Goal")
    void SetGoalLocation(FVector location) { mGoalLocation = location; }

    inline UBehaviorTree* GetBehaviorTree() { return mBehaviorTree; }

    void MoveLeftRight(bool moveLeft, bool moveRight);

    void NeuralNetworkFeedForward();
    void NeuralNetworkBackPropagate();
    TArray<bool> NeuralNetworkGetOutputValues();
    void SetNeuralNetworkInputValue(NNInputType type, bool collision);
};
