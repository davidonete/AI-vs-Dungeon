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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

    /** Called for side to side input */
    void MoveRight(float Val);

    void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    UNeuralNetworkComponent* NeuralNetworkComponent;

    float LastMovementValue = 0.0f;

public:
    ANNCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    void MoveLeftRight(bool moveLeft, bool moveRight);

    void NeuralNetworkFeedForward();
    void NeuralNetworkBackPropagate();
    TArray<bool> NeuralNetworkGetOutputValues();
    void SetNeuralNetworkInputValue(NNInputType type, bool collision);

    UPROPERTY(EditDefaultsOnly, Category = "Behavior")
    class UBehaviorTree* BehaviorTree;
};
