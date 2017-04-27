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
     int32 GetGenerations() { return mGenerations; }
     inline void SetGenerations(int32 generations) { mGenerations = generations; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     int32 GetPopulationMember() { return mPopulationMember; }
     inline void SetPopulationMember(int32 member) { mPopulationMember = member; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetShowDebugData(bool show) { mShowDebugData = show; }
     UFUNCTION(BlueprintCallable, category = "GameInstance")
     bool GetShowDebugData() { return mShowDebugData; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetInitialConfigValues(bool value) { mInitialConfigValues = value; }
     bool GetInitialConfigValues() { return mInitialConfigValues; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetEntitiesSpawned(int32 number) { mEntitiesSpawned = number; }
     UFUNCTION(BlueprintCallable, category = "GameInstance")
     int32 GetEntitiesSpawned() { return mEntitiesSpawned; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetPopulationSize(int32 size) { mPopulationSize = size; }
     int32 GetPopulationSize() { return mPopulationSize; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetCrossoverRate(int32 rate) { mCrossoverRate = rate; }
     int32 GetCrossoverRate() { return mCrossoverRate; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetMutationRate(int32 rate) { mMutationRate = rate; }
     int32 GetMutationRate() { return mMutationRate; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetMaxPerturbation(int32 rate) { mMaxPerturbation = rate; }
     int32 GetMaxPerturbation() { return mMaxPerturbation; }

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     void SetElitismRate(int32 rate) { mElitismSelection = rate; }
     int32 GetElitismRate() { return mElitismSelection; }

  private:
      int32 mFitness;
      int32 mGenerations;
      int32 mPopulationMember;
      bool mShowDebugData;

      //Initial config values
      bool mInitialConfigValues = false;
      int32 mEntitiesSpawned;
      int32 mPopulationSize;
      int32 mCrossoverRate;
      int32 mMutationRate;
      int32 mMaxPerturbation;
      int32 mElitismSelection;
};
