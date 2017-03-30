#pragma once

#include "Engine/GameInstance.h"
#include "AI_vs_DungeonGameInstance.generated.h"

UCLASS()
class AI_VS_DUNGEON_API UAI_vs_DungeonGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
 public:
     UAI_vs_DungeonGameInstance();

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     int32 GetBestFitness() { return mFitness; }
     inline void SetBestFitness(int32 fitness) { mFitness = fitness; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     int32 GetTrainingSessions() { return mTrainingSessions; }
     inline void SetTrainingSessions(int32 sessions) { mTrainingSessions = sessions; }

  private:
      int32 mFitness;
      int32 mTrainingSessions;
};
