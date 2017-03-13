
#include "AI_vs_Dungeon.h"
#include "Runtime/Engine/Public/Engine.h"
#include "Game/AI_vs_DungeonGameInstance.h"
#include "NNCharacter.h"

ANNCharacter::ANNCharacter()
{
    //Enable tick
    PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->RelativeRotation = FRotator(0.f,180.f,0.f);

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

    NeuralNetworkComponent = CreateDefaultSubobject<UNeuralNetworkComponent>(TEXT("Neural Network Component"));
}

void ANNCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveRight(LastMovementValue);
}

void ANNCharacter::SetNeuralNetworkInputValue(NNInputType type, bool collision)
{
    uint16 index = (uint16)type;
    double value;

    if (collision) value = 1.0;
    else value = 0.0;

    NeuralNetworkComponent->SetInputValue(index, value);
}

void ANNCharacter::MoveLeftRight(bool moveLeft, bool moveRight)
{
    if (moveLeft) LastMovementValue = -1.0f;
    if (moveRight) LastMovementValue = 1.0f;
    if (!moveLeft && !moveRight) LastMovementValue = 0.0f;
}

void ANNCharacter::NeuralNetworkFeedForward()
{
    NeuralNetworkComponent->PrintInputValues();
    NeuralNetworkComponent->FeedForward();
}

void ANNCharacter::NeuralNetworkBackPropagate()
{
    TArray<double> inputValues = NeuralNetworkComponent->GetInputValues();
    TArray<double> targetValues = NeuralNetworkComponent->GetTargetValues(inputValues);

    NeuralNetworkComponent->PrintArray("Target: ", targetValues);

    NeuralNetworkComponent->BackPropagate(targetValues);
}

TArray<bool> ANNCharacter::NeuralNetworkGetOutputValues()
{
    TArray<double> outputValues;
    NeuralNetworkComponent->GetResults(outputValues);

    NeuralNetworkComponent->PrintArray("Out: ", outputValues);

    TArray<bool> result;
    for (uint16 i = 0; i < outputValues.Num(); i++)
    {
        if (outputValues[i] > 0.5)
            result.Add(true);
        else
            result.Add(false);
    }

    UAI_vs_DungeonGameInstance *gameInstance = Cast<UAI_vs_DungeonGameInstance>(GetGameInstance());
    if (gameInstance)
        gameInstance->SetAverageError((float)NeuralNetworkComponent->GetRecentAverageError());

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANNCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	//InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//InputComponent->BindAxis("MoveRight", this, &ANNCharacter::MoveRight);
}

void ANNCharacter::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}