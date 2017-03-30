#include "AI_vs_Dungeon.h"
#include "GeneticAlgorithmController.h"

// Sets default values
AGeneticAlgorithmController::AGeneticAlgorithmController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    mGAComponent = CreateDefaultSubobject<UGeneticAlgorithmComponent>(TEXT("Genetic Algorithm Component"));

    mGenomeIndex = 0;
}

// Called when the game starts or when spawned
void AGeneticAlgorithmController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGeneticAlgorithmController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGeneticAlgorithmController::SpawnEntity()
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ANNCharacter* entity = GetWorld()->SpawnActor<ANNCharacter>(mEntity, GetActorLocation(), GetActorRotation(), SpawnParams);
    
    if (entity)
    {
        int32 GenomeID;
        entity->NeuralNetworkInitialize();

        //Create a entity with a random genome until the base genome population size is reached
        if (mGAComponent->GetPopulationSize() < mGAComponent->GetMaxPopulationSize())
        {
            GenomeID = mGAComponent->NewGenome(entity);
            mGenomeIndex++;
        }
        else
        {
            //If all the genomes have been created and tested, we create the new generation
            if (mGenomeIndex > mGAComponent->GetPopulationSize() - 1)
            {
                mGenomeIndex = 0;
                mGAComponent->Epoch();
            }

            //Create a new entity with the a genome of the new generation
            GenomeID = mGenomeIndex;
            SGenome genome = mGAComponent->GetGenome(mGenomeIndex);

            //Update the NN weights with the new ones obtained from crossover and mutation
            entity->NeuralNetworkSetConnectionWeights(genome.Bits);

            mGenomeIndex++;
        }
        entity->SetGeneticAlgorithmController(GenomeID, this);
    }
}

void AGeneticAlgorithmController::UpdateEntityFitness(int32 id, double fitness)
{
    UE_LOG(LogTemp, Warning, TEXT("Entity Fitness: %f"), (float)fitness);
    mGAComponent->UpdateGenomeFitness(id, fitness);
}